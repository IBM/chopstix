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

struct TraceBuffer {
  public:
    ~TraceBuffer();

    void setup(const char *trace_root);

    void start_trace(int trace_id, bool isNewInvocation);
    void stop_trace(int trace_id);
    void save_page(long page_addr);
    void write_back();

  private:
    static constexpr long buf_size = 4096;
    void write(long dat);

    long pos_ = 0;
    long buf_[buf_size];
    int fd_ = -1;
};

}  // namespace chopstix
