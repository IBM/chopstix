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
#include "buffer.h"

#include "common/format.h"
#include "common/check.h"


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>

using namespace cxtrace;

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

TraceBuffer::~TraceBuffer() {
    if (pos_ > 0) write_back();
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

void TraceBuffer::setup(const char *trace_root) {
    char fpath[PATH_MAX];
    fmt::format(fpath, sizeof(fpath), "%s/trace.bin", trace_root);
    fd_ = ::creat(fpath, PERM_664);
    check(fd_ != -1, "Unable to open 'trace.bin'");
}

void TraceBuffer::start_trace(int trace_id) {
    write(-1);
}

void TraceBuffer::stop_trace(int trace_id) {
    write(-2);
}

void TraceBuffer::save_page(long page_addr) {
    write(page_addr);
}

void TraceBuffer::write_back() {
    ::write(fd_, buf_, pos_ * sizeof(long));
    pos_ = 0;
}

void TraceBuffer::write(long dat) {
    if (pos_ == buf_size) write_back();
    buf_[pos_++] = dat;
}


