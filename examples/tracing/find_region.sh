#!/usr/bin/env sh
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

die () {
    echo "$@" >&2
    exit
}

[ $# -ge 2 ] || die "Usage: $0 <binary> <function>"

# First find begin address
objdump -d "$1" | awk "/<$2>:/ { print \"-begin\",\$1 }"

# Find all end addresses (we take the instructions after a call to a function)
objdump -d "$1" | awk "
/<$2>\$/ { flag=1 ; next }
flag { gsub(/:/,\"\",\$1); end=end\",\"\$1 ; flag=0 }
END { gsub(/^,/,\"\",end); print \"-end\",end }"
