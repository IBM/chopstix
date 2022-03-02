/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 IBM Corporation
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
*/

.section .text
.align 2
.globl sandwich_return
.type sandwich_return,@function
sandwich_return:
    b .L2
.L1:
    blr
    .4byte 0xdeadbeef
.L2:
    lis 3,.L1@ha
    la 3,.L1@l(3)
    ld 3, 0(3)
    b .L1
