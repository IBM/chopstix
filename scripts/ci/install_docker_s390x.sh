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

sudo service docker start
git clone https://github.com/dockcross/dockcross
cd dockcross
docker run --rm dockcross/linux-s390x > ./dockcross-linux-s390x
chmod +x dockcross-linux-s390x
cp dockcross-linux-s390x /usr/bin/
cd - || exit 1

for elem in $(dockcross-linux-s390x ls /usr/xcc/s390x-ibm-linux-gnu/bin/); do

    echo "#!/usr/bin/env sh" > "/usr/bin/$elem"
    echo "extra_dirs=\$(for elem in \$(find $(pwd) -type d | grep -v .git); do echo -v \$elem:\$elem; done;)" >> "/usr/bin/$elem"
    echo "dockcross-linux-s390x --args \"-v /tmp/libpfm/:/tmp/libpfm/ \$extra_dirs -v \$(pwd):\$(pwd) -v /usr/share:/usr/share \" -- bash -c \" cd \$(pwd) && /usr/xcc/s390x-ibm-linux-gnu/bin/$elem \$* \"" >> "/usr/bin/$elem"
    chmod +x "/usr/bin/$elem"
    echo "/usr/bin/$elem created!"

done;

# vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab
