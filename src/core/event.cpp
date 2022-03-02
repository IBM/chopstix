/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2019 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ----------------------------------------------------------------------------
#
*/
/******************************************************************************
 * NAME        : core/event.cpp
 * DESCRIPTION : Wrapper around perf_event
 ******************************************************************************/

#include "event.h"

// Language headers
#include <cstring>

// System headers
#include <sys/mman.h>
#include <unistd.h>

// Application headers
#include "perfmon.h"
#include "support/check.h"
#include "support/string.h"

using namespace chopstix;

Event::Event(std::string name) : name_(std::move(name)) {
    log::debug("Event create start: %s", name_.c_str());
    // Lazy initialization of libpfm
    // when first event is created
    Perfmon::Get_instance().initialize();

    memset(&attr_, 0, sizeof(attr_));
    attr_.size = sizeof(attr_);  // call before calling libpfm

    pfm_perf_encode_arg_t arg;
    memset(&arg, 0, sizeof(arg));

    char *fstr = nullptr;
    arg.attr = &attr_;
    arg.fstr = &fstr;
    arg.size = sizeof(pfm_perf_encode_arg_t);

    int ret = pfm_get_os_event_encoding(name_.c_str(), PFM_PLM0 | PFM_PLM3,
                                        PFM_OS_PERF_EVENT_EXT, &arg);
    if (ret != PFM_SUCCESS) {
        log::error("Unable to get encoding for event '%s'\n", name_);

        pfm_event_info_t info;
        pfm_pmu_info_t pinfo;
        int i, ret;

        memset(&info, 0, sizeof(info));
        memset(&pinfo, 0, sizeof(pinfo));

        info.size = sizeof(info);
        pinfo.size = sizeof(pinfo);

        pfm_pmu_t pmu = PFM_PMU_PERF_EVENT;

        ret = pfm_get_pmu_info(pmu, &pinfo);
        checkx(ret == PFM_SUCCESS, "cannot get pmu info");

        for (i = pinfo.first_event; i != -1; i = pfm_get_event_next(i)) {
            ret = pfm_get_event_info(i, PFM_OS_PERF_EVENT_EXT, &info);
            checkx(ret == PFM_SUCCESS, "cannot get event info");

            log::error("%s Event: PMU %s:: Name: %s",
                      pinfo.is_present ? "Active" : "Supported",
                      pinfo.name, info.name);
        }
        checkx(false,
           "Unable to get encoding for event '%s'\n", name_);
    }

    info_ = std::string(fstr);
    free(fstr);

    log::debug("Event create end: %s", name_.c_str());
}

Event::~Event() {
    if (is_open()) {
        close();
    }
    if (is_buffering()) {
        stop_buffering();
    }
}

Event::Event(Event &&other)
    : name_(std::move(other.name_)),
      attr_(std::move(other.attr_)),
      fd_(other.fd_),
      info_(std::move(other.info_)) {
    other.fd_ = -1;
}

Event &Event::operator=(Event &&other) {
    if (this != &other) {
        name_ = std::move(other.name_);
        attr_ = std::move(other.attr_);
        fd_ = other.fd_;
        other.fd_ = -1;
        info_ = std::move(other.info_);
    }
    return *this;
}

void Event::setup() {
    attr_.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED |
                        PERF_FORMAT_TOTAL_TIME_RUNNING | PERF_FORMAT_GROUP;
    attr_.sample_type =
        PERF_SAMPLE_IP | PERF_SAMPLE_TID | PERF_SAMPLE_TIME | PERF_SAMPLE_READ;
    if (!attr_.sample_period) {
        set_period(1000000);
    }
    if (!attr_.wakeup_watermark) {
        set_watermark(buf_size_ * sysconf(_SC_PAGESIZE) / 2);
    }
    attr_.exclude_kernel = 1;
}

void Event::open(pid_t pid, int cpu, int group_fd, unsigned long flags) {
    log::debug("Event::open start");
    log::debug("Event::open pid: %d", pid);
    log::debug("Event::open cpu: %d", cpu);
    log::debug("Event::open group_fd: %d", group_fd);
    log::debug("Event::open flags: %d", flags);
    log::debug("Event::open attr: %x", &attr_);
    log::debug("Event::open start");
    fd_ = perf_event_open(&attr_, pid, cpu, group_fd, flags);
    if (is_open()) return;
    switch (errno) {
        case E2BIG:
            fail("E2BIG: Size error");
        case EACCES:
            fail("EACCES: Unable to open event %s: %s\n"
                "You need to be sysadmin or change your settings\n"
                "in /proc/sys/kernel/perf_event_paranoid.",
                name_, strerror(errno));
        case EBADF:
            fail("EBADF: group_fd file descriptor is not valid");
        case EBUSY:
            fail("EBUSY: another event already has exclusive access to the PMU");
        case EFAULT:
            fail("EFAULT: attr pointer points at an invalid memory address");
        case EINVAL:
            fail(
                "Unable to open event %s (%s: %d): %s\n"
                "Your parameters might be wrong or this event is not "
                "supported.\n"
                "Check '--period', '--freq', or '--events'\n"
                "See 'perf list' for a list of available events.",
                name_, (attr_.freq ? "freq" : "period"),
                (attr_.freq ? attr_.sample_freq : attr_.sample_period),
                strerror(errno));
        case EMFILE:
            fail("EMFILE: no more events can be created");
        case ENODEV:
            fail("ENODEV: feature not supported by the current CPU");
        case ENOENT:
            fail("ENOENT: type setting is not valid. Also returned for some unsupported generic events");
        case ENOSPC:
            fail("ENOSPC: no more breakpoints events supported");
        case ENOSYS:
            fail("ENOSYS: PERF_SAMPLE_STACK_USER not supported by the hardware");
        case EOPNOTSUPP:
            fail("EOPNOTSUPP: feature not supported by the hardware");
        case EOVERFLOW:
            fail("EOVERFLOW: overflow in sample_max_stack during PERF_SAMPLE_CALLCHAIN");
        case EPERM:
            fail(
                "EPERM: Unable to open event %s: %s\n"
                "You need to be sysadmin or change your settings\n"
                "in /proc/sys/kernel/perf_event_paranoid.",
                name_, strerror(errno));
        case ESRCH:
            fail("ESRCH: attempting to attach to a process that does not exist");

        default: fail("Unable to open event %s: %s (error: %d)\n", name_, strerror(errno), errno);
    }
}

void Event::close() {
    ::close(fd_);
    fd_ = -1;
}

void Event::set_freq(uint64_t freq) {
    if (!is_open()) {
        attr_.sample_freq = freq;
        attr_.freq = 1;
    }
}

void Event::set_period(uint64_t period) {
    if (!is_open()) {
        attr_.sample_period = period;
        attr_.freq = 0;
    }
}

void Event::set_watermark(uint64_t wmark) {
    if (!is_open()) {
        attr_.wakeup_watermark = wmark;
        attr_.watermark = 1;
    }
}

void Event::set_overflow(uint64_t ovf) {
    if (!is_open()) {
        attr_.wakeup_events = ovf;
        attr_.watermark = 0;
    }
}

void Event::enable() {
    if (is_open()) {
        int ret = ioctl(fd_, PERF_EVENT_IOC_ENABLE, 0);
        check(ret == 0, "Unable to enable event '%s'", name_);
    } else {
        attr_.disabled = 0;
    }
}

void Event::disable() {
    if (is_open()) {
        int ret = ioctl(fd_, PERF_EVENT_IOC_DISABLE, 0);
        check(ret == 0, "Unable to disable event '%s'", name_);
    } else {
        attr_.disabled = 1;
    }
}

void Event::enable_on_exec() {
    if (!is_open()) {
        attr_.disabled = 1;
        attr_.enable_on_exec = 1;
    }
}

Event::value_list Event::read(size_t len) {
    checkx(is_open(), "Event '%s' is not open", name_);

    ssize_t rd_size = (len + 3) * sizeof(uint64_t);
    value_list rd(len + 3);
    ssize_t ret = ::read(fd_, rd.data(), rd_size);
    check(ret == rd_size, "Unable to read event '%s'", name_);

    uint64_t nr = rd[0];
    uint64_t time_enabled = rd[1];
    uint64_t time_running = rd[2];
    checkx(nr == len, "Missmatched event length. Restart sampling");

    for (size_t i = 3; i < len + 3; ++i) {
        rd[i] = rd[i] * time_enabled / time_running;
    }
    return value_list(rd.begin() + 3, rd.end());
}

size_t Event::map_size() const {
    return (buf_size_ + 1) * sysconf(_SC_PAGESIZE);
}

void Event::start_buffering() {
    if (is_buffering()) {
        return;
    }

    checkx(attr_.sample_freq, "No sampling freq or period set");

    buf_ = mmap(nullptr, map_size(), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_LOCKED, fd(), 0);

    if(buf_ == MAP_FAILED) {
        buf_ = mmap(nullptr, map_size(), PROT_READ | PROT_WRITE,
                MAP_SHARED, fd(), 0);
    }

    check(buf_ != MAP_FAILED, "Unable to map buffer to memory");
    pfd_.fd = fd();
    pfd_.events = POLLIN;
}

void Event::stop_buffering() {
    if (!is_buffering()) {
        return;
    }
    munmap(buf_, map_size());
    buf_ = nullptr;
}

bool Event::poll(int timeout) {
    if (!is_buffering()) {
        return false;
    }
    int ret = ::poll(&pfd_, 1, timeout);
    if (ret < 0 && errno == EINTR) {
        return false;
    }
    return true;
}

void Event::set_buf_size(int pages) {
    checkx(pages > 0, "Attempt to set negative number of pages");
    buf_size_ = pages;
}

uint64_t Event::freq() const { return attr_.freq ? attr_.sample_freq : 0; }
uint64_t Event::period() const { return attr_.freq ? 0 : attr_.sample_period; }
uint64_t Event::watermark() const {
    return attr_.watermark ? attr_.wakeup_watermark : 0;
}
uint64_t Event::overflow() const {
    return attr_.watermark ? 0 : attr_.wakeup_events;
}

void Event::read_buffer(void *bf, size_t sz) {
    check(is_buffering(), "No buffer available");
    check(avail() >= sz, "No data available");
    static const size_t page_size = sysconf(_SC_PAGESIZE);
    static const size_t page_mask = buf_size_ * page_size - 1;
    auto *hdr = page();
    char *dat = (char *)hdr + page_size;
    unsigned long tail;
    size_t part_sz, left_sz;

    // position of tail
    tail = hdr->data_tail & page_mask;

    // size till end of buffer
    left_sz = page_mask + 1 - tail;
    part_sz = std::min(left_sz, sz);

    // copy beginning
    memcpy(bf, dat + tail, part_sz);
    // copy wrapped around leftover
    if (sz > part_sz) {
        memcpy((char *)bf + part_sz, dat, sz - part_sz);
    }

    // update header
    hdr->data_tail += sz;
}

void Event::skip_buffer(size_t sz) {
    auto *hdr = page();
    if ((hdr->data_tail + sz) > hdr->data_head) {
        sz = hdr->data_head - hdr->data_tail;
    }
    hdr->data_tail += sz;
}

std::vector<Sample> Event::sample() {
    std::vector<Sample> samples;
    for (;;) {
        if (avail() < sizeof(header_type)) {
            break;
        }

        header_type hdr;
        read_buffer(&hdr);

        if (hdr.type == PERF_RECORD_SAMPLE) {
            Sample smp;
            read_buffer(&smp, Sample::header_size);
            uint64_t nr;
            read_buffer(&nr);
            smp.data.resize(nr);
            uint64_t timing[2];
            read_buffer(&timing, 2 * sizeof(uint64_t));
            read_buffer(smp.data.data(), nr * sizeof(uint64_t));
            for (auto &dat : smp.data) {
                dat *= timing[0] / timing[1];
            }
            num_samples_ += 1;
            samples.push_back(smp);
        } else if (hdr.type == PERF_RECORD_LOST) {
            uint64_t lost;
            skip_buffer(sizeof(uint64_t));  // id;
            read_buffer(&lost);
            skip_buffer(hdr.size - sizeof(hdr) - 2 * sizeof(uint64_t));
            num_lost_ += lost;
        } else {
            skip_buffer(hdr.size - sizeof(hdr));
        }
    }
    return samples;
}

std::vector<Event> Event::parse_all(const std::string &desc) {
    std::vector<Event> events;
    for (auto &str : string::splitg(desc, ", ")) {
        events.push_back(Event(str));
    }
    return events;
}
