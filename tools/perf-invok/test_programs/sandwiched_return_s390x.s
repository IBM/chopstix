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

.text
	.align	8
	.align	16
.globl sandwich_return
	.type	sandwich_return, @function
sandwich_return:
    j .L2
.L1: 
	br	%r14
    .byte 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad
.L2:
    larl %r2, .L1
    lg %r2, 0(%r2)
    j .L1
