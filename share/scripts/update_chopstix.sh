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

die() {
    echo "$@" >&2
    exit 1
}

[ -z "$CHOPSTIX_VERSION" ] && die "ChopStix not setup correctly"

test_cmd() {
    "$@" >/dev/null 2>&1 || die "$1 not found"
}

test_cmd git --version
test_cmd cmake --version
test_cmd make --version

cd "$CHOPSTIX_SOURCE"
git diff-index --quiet HEAD -- || die "Source directory not clean\\nPlease commit changes"

branch=$(git branch | awk '/\*/{print $2}')

cleanup() {
    cd "$CHOPSTIX_SOURCE"
    git checkout "$branch"
}

[ "$branch" = "master" ] || git checkout master

git fetch

logremote=$(git log origin/master..master)
loglocal=$(git log master..origin/master)

if [ "$logremote" = "" ] ; then
    cleanup
    die "Local branch ahead of origin\\nPlease push to origin"
fi

if [ "$loglocal" = "" ] ; then
    echo "Already up to date"
    cleanup
    exit 0
else
    echo "Updating local branch"
    git pull origin master
fi

cd "$CHOPSTIX_BUILD"
echo "Building ChopStix"
make install || cleanup && die "Unable to build ChopStix"

cleanup
echo "Build complete"
echo "Run 'source \$CHOPSTIX_SHARE/setup.sh' to complete update"
