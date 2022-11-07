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
#include "membuffer.h"

#include "support/check.h"
#include "support/safeformat.h"

#include <fcntl.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/syscall.h>

using namespace chopstix;

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

MemBuffer::~MemBuffer() {
    if (pos_ > 0) write_back();
    if (fd_ != -1) {
        syscall(SYS_close, fd_);
        fd_ = -1;
    }
}

void MemBuffer::setup(const char *trace_root) {
    char fpath[PATH_MAX];
    sfmt::format(fpath, sizeof(fpath), "%s/memory_trace.bin", trace_root);
    fd_ = syscall(SYS_openat, AT_FDCWD, fpath, O_WRONLY | O_CREAT | O_TRUNC, PERM_664);

    check(fd_ != -1, "Unable to open 'memory_trace.bin'");
}

void MemBuffer::start_trace(int trace_id, bool isNewInvocation) { if(isNewInvocation) write(-3, 0, 0, 0); write(-1, 0, 0, 0); }
void MemBuffer::stop_trace(int trace_id) { write(-2, 0, 0, 0); }
void MemBuffer::save_mem_read(long src_addr, long target_addr) { save_mem_access(src_addr, target_addr, 0); }
void MemBuffer::save_mem_write(long src_addr, long target_addr) { save_mem_access(src_addr, target_addr, 1); }
void MemBuffer::save_code_read(long src_addr, long target_addr) { save_code_access(src_addr, target_addr, 0); }
void MemBuffer::save_code_write(long src_addr, long target_addr) { save_code_access(src_addr, target_addr, 1); }
void MemBuffer::save_mem_access(long src_addr, long target_addr, long type) { write(src_addr, target_addr, type, 0); }
void MemBuffer::save_code_access(long src_addr, long target_addr, long type) { write(src_addr, target_addr, type, 1); }

void MemBuffer::write_back() {
    syscall(SYS_write, fd_, buf_, pos_ * sizeof(long));
    pos_ = 0;
}

void MemBuffer::write(long src_addr, long target_addr, long type, long type2) {
    if (pos_ > 0) {
        if ((buf_[pos_-4] == src_addr) && (buf_[pos_-3] == target_addr) && (buf_[pos_-2] == 0) && (type == 1)) {
            buf_[pos_-2] = 1;
            return;
        }
    }
    if (pos_ == buf_size) write_back();
    buf_[pos_] = src_addr;
    pos_ += 1;
    buf_[pos_] = target_addr;
    pos_ += 1;
    buf_[pos_] = type;
    pos_ += 1;
    buf_[pos_] = type2;
    pos_ += 1;
}

