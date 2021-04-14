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

if [ $# -eq 0 ]; then
    echo "Using default install directory: /tmp/zlib"
    dir=/tmp/zlib
fi

if [ $# -eq 1 ]; then
    dir=$1
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

tmp="$dir/src_build/"
mkdir -p "$tmp"
cd "$tmp" 

ZLIB_VERSION=1.2.11
rm -f zlib-${ZLIB_VERSION}.tar.gz
wget https://www.zlib.net/zlib-${ZLIB_VERSION}.tar.gz
tar xvf zlib-${ZLIB_VERSION}.tar.gz

cd zlib-${ZLIB_VERSION} 

./configure --prefix /tmp/zlib
cat configure.log

make -j 
make install

cd - || exit 1

rm -fr "$tmp"

# vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab
