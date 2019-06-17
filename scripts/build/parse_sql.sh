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

############################################################
# NAME        : parse_sql.sh
# DESCRIPTION : Transform SQL files into C headers
# ARGUMENTS   :
#    in  : Input SQL file
#    out : Output header file
############################################################

in=$1
out=$2
base=$(basename "$in" .sql)
BASE=$(echo "$base" | tr /a-z/ /A-Z/ | tr '-' '_')
outdir=$(dirname "$out")
mkdir -p "$outdir"

{
echo "/***********************************************************"
awk '/^-- /{ gsub(/^-- /," * "); print }' "$in"
echo " ***********************************************************/"
echo ""
echo "#pragma once"
echo ""
awk '/^@/ { gsub(/@/,"",$0); print "#include \"sql/"$0".h\"" }' "$in"
echo ""
echo "#define SQL_$BASE \\"
awk '
/^$/ { next }
/^--/ { next }
/^@/ { gsub(/@/,"",$0); print "    SQL_"toupper($0) " \\"; next }
1 { print "    \"  "$0"\\n\" \\" }
' "$in"
echo ""
} > "$out"

