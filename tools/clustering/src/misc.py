#!/usr/bin/env python3
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 CHOPSTIX Authors
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
import bz2
import gzip


def open_generic_fd(filename, mode):

    if filename.endswith(".gz"):
        if "b" not in mode:
            mode += "b"
        fd = gzip.open(filename, mode, compresslevel=9)
    elif filename.endswith(".bz2"):
        if "b" not in mode:
            mode += "b"
        fd = bz2.BZ2File(filename, mode, compresslevel=9)
    else:
        fd = open(filename, mode)

    return fd
