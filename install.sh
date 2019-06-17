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
# 
# ChopStiX CI support scripts
#
# Author: Ramon Bertran Monfort <rbertra@us.ibm.com>
#
# Copyright 2019 IBM Corporation
# IBM (c) 2019 All rights reserved
#

set -e # Finish right after a non-zero return command
set -u # Finish right after a undefined expression is used
set -a # All following variables are exported

base_dir=$(readlink -m "$0")
base_dir=$(dirname "$base_dir")

if [ $# -eq 0 ]; then
    echo "Using default install directory: /tmp/chopstix"
    dir=/tmp/libpfm
fi

if [ $# -eq 1 ]; then
    dir="$1"
    dir=$(readlink -m "$dir")
elif [ $# -gt 1 ]; then
    echo "Usage: $0 <install_dir>"
    exit 1
fi

if [ ! -d "$dir" ]; then
    echo "Install directory '$dir' does not exists"
    set +e
    mkdir -p "$dir"
    error=$?
    set -e
    if [ $error -ne 0 ]; then
        echo "Unable to create '$dir' install directory"
        exit 1
    fi
fi

tmp=$(mktemp -d)
cd "$tmp"

"$base_dir/scripts/ci/install_libpfm.sh" "$dir"
cmake "$base_dir" -DCHOPSTIX_PERFMON_PREFIX="$dir" -DCMAKE_INSTALL_PREFIX="$dir" -DCHOPSTIX_BUILD_SQLITE=ON
make -j 
make -j install

cd - || exit 1
rm -fr "$tmp"

echo "ChopStiX installed in $dir" 
echo "Execute: 'source $dir/share/chopstix/setup.sh' to set up the ChopStix environment"

# vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab
