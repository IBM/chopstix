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

set -eu

set_version() {
    major=$(echo "$1" | awk 'BEGIN{FS="."} {print $1}')
    minor=$(echo "$1" | awk 'BEGIN{FS="."} {print (NF>1)?$2:"0"}')
    patch=$(echo "$1" | awk 'BEGIN{FS="."} {print (NF>2)?$3:"0"}')

    sed -i "s/set\\s*(\\s*CHOPSTIX_VERSION_MAJOR\\s\\s*.*)/set (CHOPSTIX_VERSION_MAJOR $major)/" CMakeLists.txt
    sed -i "s/set\\s*(\\s*CHOPSTIX_VERSION_MINOR\\s\\s*.*)/set (CHOPSTIX_VERSION_MINOR $minor)/" CMakeLists.txt
    sed -i "s/set\\s*(\\s*CHOPSTIX_VERSION_PATCH\\s\\s*.*)/set (CHOPSTIX_VERSION_PATCH $patch)/" CMakeLists.txt
}

get_version() {
    awk '
    /set\s*\(\s*CHOPSTIX_VERSION_/ {
        gsub(/set\s*\(\s*CHOPSTIX_VERSION_[^ ]*/, "")
        gsub(/\)/, "")
        gsub(/\s/, "")
        ver=ver"."$0
    }
    END {
        gsub(/^./, "", ver)
        print ver
    }
    ' CMakeLists.txt
}

if [ $# -gt 0 ]
then
    set_version "$1"
else
    echo "Current version: $(get_version)"
    printf "New version: "
    read -r ver
    set_version "$ver"
fi


