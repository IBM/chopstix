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
 * NAME        : core/event.h
 * DESCRIPTION : Wrapper around perf_event
 ******************************************************************************/

// Language headers
#include <cinttypes>
#include <string>
#include <vector>

// System headers
#include <perfmon/pfmlib_perf_event.h>
#include <poll.h>

// Private headers
#include "sample.h"

namespace chopstix {

class Event {
  public:
    typedef struct perf_event_header header_type;
    typedef struct perf_event_mmap_page page_type;
    typedef struct perf_event_attr attr_type;
    typedef std::vector<uint64_t> value_list;

    Event(std::string name);
    ~Event();

    Event(const Event &) = delete;
    Event &operator=(const Event &) = delete;
    Event(Event &&);
    Event &operator=(Event &&);

    const std::string &name() const { return name_; }
    const attr_type &attr() const { return attr_; }
    int fd() const { return fd_; }
    const std::string &info() const { return info_; }

    // Set attr flags
    void setup();

    void open(pid_t pid = 0, int cpu = -1, int group_fd = -1,
              unsigned long flags = 0);
    void close();
    bool is_open() const { return fd_ != -1; }

    void enable();
    void disable();
    void enable_on_exec();

    value_list read(size_t len = 1);

    void start_buffering();
    void stop_buffering();
    bool is_buffering() const { return buf_ != nullptr; }

    bool poll(int timeout = 0);
    std::vector<Sample> sample();

    // Helpers
    void set_freq(uint64_t);
    void set_period(uint64_t);
    void set_watermark(uint64_t);
    void set_overflow(uint64_t);
    uint64_t freq() const;
    uint64_t period() const;
    uint64_t watermark() const;
    uint64_t overflow() const;

    int buf_size() const { return buf_size_; }
    void set_buf_size(int);

    uint64_t num_lost() const { return num_lost_; }
    uint64_t num_samples() const { return num_samples_; }

    size_t map_size() const;

    std::string repr() const { return info(); }
    std::string str() const { return name(); }

    template <typename T>
    void read_buffer(T *bf) {
        return read_buffer(bf, sizeof(T));
    }
    void read_buffer(void *, size_t);
    void skip_buffer(size_t);
    page_type *page() const { return (page_type *)buf_; }
    size_t avail() const { return page()->data_head - page()->data_tail; }

    static std::vector<Event> parse_all(const std::string &);

  private:
    std::string name_;
    attr_type attr_;
    int fd_ = -1;

    std::string info_;

    void *buf_ = nullptr;
    struct pollfd pfd_;

    uint64_t num_lost_ = 0;
    uint64_t num_samples_ = 0;

    int buf_size_ = 32;
};

}  // namespace chopstix
