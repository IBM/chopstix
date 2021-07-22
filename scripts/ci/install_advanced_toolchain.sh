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

ATVERSION=12.0
DISTRO=xenial
UPDATE=5

if [ ! -d ./toolchain ]; then
    mkdir ./toolchain
fi

cd ./toolchain

apt-get install axel 
baseurl="https://public.dhe.ibm.com/software/server/POWER/Linux/toolchain/at/ubuntu/dists/$DISTRO/at$ATVERSION/binary-amd64/"
packages="advance-toolchain-at$ATVERSION-cross-common_$ATVERSION-${UPDATE}_amd64.deb
          advance-toolchain-at$ATVERSION-cross-ppc64_$ATVERSION-${UPDATE}_amd64.deb
          advance-toolchain-at$ATVERSION-cross-ppc64-mcore-libs_$ATVERSION-${UPDATE}_amd64.deb
          advance-toolchain-at$ATVERSION-cross-ppc64-runtime-extras_$ATVERSION-${UPDATE}_amd64.deb"

for package in $packages; do
    if [ ! -f "./$package" ]; then 
        echo "Downloading $package ..."
        axel -n 8 -q "$baseurl/$package"
    else
        echo "$package already in cache"
    fi

    echo "Installing packages ..."
    set +e
    dpkg -i "./$package"
    apt-get install -f -y
    set -e
done;

# echo "Installing packages ..."
# set +e
# dpkg -R -i . 
# apt-get install -f -y
# set -e

PATH=$PATH:/opt/at$ATVERSION/bin 
export PATH
command -v powerpc64-linux-gnu-gcc

cd - || exit 1

# vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab
