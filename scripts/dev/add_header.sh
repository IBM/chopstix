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

set -e # Finish right after a non-zero return command
set -u # Finish right after a undefined expression is used
set -a # All following variables are exported

hfile="./scripts/dev/header"

if [ ! -f "$hfile" ]; then
    echo "'$0' should be executed from base directory"
    exit 1
fi

# shellcheck disable=SC2048
for file in $* ; do
    echo "Processing '$file'"
    if [ ! -f "$file" ]; then
        echo "'$file' does not exist"
        exit 1
    fi

    if [ "$(grep -c "Licensed under the Apache License, Version 2.0 (the \"License\");" "$file")" -gt 0 ]; then
        echo "'$file' already has a header"
        continue
    fi

    filenew="$file.new"
    rm -f "$filenew"

    ccode=0
    for ext in c cpp h hpp; do
        if [ "$(echo "$file" | grep -c ".$ext$")" -gt 0 ]; then
            echo "'$file' is C code"
            ccode=1
            break
        fi
    done;

    if [ "$(head -n 1 "$file" | grep -c "^#!")" -gt 0 ]; then
        echo "'$file' has an interpreter"
        head -n 1 "$file" >> "$filenew"
    fi

    if [ "$ccode" -ne 0 ]; then
        echo "/*" >> "$filenew"
    fi
    cat "$hfile" >> "$filenew"
    if [ "$ccode" -ne 0 ]; then
        echo "*/" >> "$filenew"
    fi

    if [ "$(head -n 1 "$file" | grep -c "^#!")" -gt 0 ]; then
        tail -n +2 "$file" | sed -e "s/Copyright 2019 IBM Corporation/Copyright 2019 IBM Corporation/g" | sed -e "s/2019 All rights reserved/2019 All rights reserved/g" >> "$filenew"
    else
        sed -e "s/Copyright 2019 IBM Corporation/Copyright 2019 IBM Corporation/g" "$file" | sed -e "s/2019 All rights reserved/2019 All rights reserved/g" >> "$filenew"
    fi

    cp -f "$file" "$file.backup"
    mv -f "$filenew" "$file"

done;
