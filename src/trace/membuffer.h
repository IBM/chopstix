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
#pragma once

namespace chopstix {

struct MemBuffer {
  public:
    ~MemBuffer();

    void setup(const char *trace_root);

    void start_trace(int trace_id, bool isNewInvocation);
    void stop_trace(int trace_id);
    void save_mem_write(long src_addr, long target_addr);
    void save_mem_read(long src_addr, long target_addr);
    void save_code_write(long src_addr, long target_addr);
    void save_code_read(long src_addr, long target_addr);
    void write_back();

  private:
    static constexpr long buf_size = 4096 / (sizeof(long)*4);
    void write(long src_addr, long target_addr, long type, long type2);
    void save_mem_access(long src_addr, long target_addr, long type);
    void save_code_access(long src_addr, long target_addr, long type);

    long pos_ = 0;
    long buf_[buf_size * (sizeof(long)*4)];
    int fd_ = -1;
};

}  // namespace chopstix
