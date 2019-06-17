#!/usr/bin/env python
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
import argparse
import subprocess
import re
import resource
from os import path


parser = argparse.ArgumentParser(description="Create mpt/mps files")
parser.add_argument("-t", "--test", type=str, default="test.mpt",
                    help="output test file")
parser.add_argument("-s", "--state", type=str, default="state.mps",
                    help="output state file")
parser.add_argument("-d", "--data", type=str, default="data",
                    help="data path")
parser.add_argument("-i", "--index", type=str, default="1",
                    help="select invocation")
parser.add_argument("--code", action="store_true", help="use code information")
parser.add_argument("--pagesize", type=int, default=resource.getpagesize())
args = parser.parse_args()

modules=[]

maps = open(path.join(args.data, "maps.{}".format(args.index)), "r")
for line in maps:
    words = line.split()
    if words[1] != "r-xp": continue
    if len(words) < 6: continue
    module = words[5]
    if module[0] == "[": continue
    if "/libcxtrace" in module: continue
    if "/libc." in module: continue
    if "/libc-" in module: continue
    if "/ld-" in module: continue
    if "/ld." in module: continue
    reg = [int(a, 16) for a in words[0].split("-")]
    modules.append((module, reg[0], reg[1]))

state=open(args.state, "w")

codepages=[]

trace=open(path.join(args.data, "trace"), "r")
for line in trace:
    index, addr, perm = line.split()
    if index != args.index: continue
    if perm == "r-xp":
        codepages.append(int(addr, 16))
    else:
        page=path.join(args.data, "page.{}.{}".format(index, addr))
        getbin=subprocess.Popen("xxd -p -c 8192 {}".format(page),
                                shell=True, stdout=subprocess.PIPE)
        bindat=getbin.stdout.readline().rstrip()
        state.write("M 0x{} {}\n".format(addr, bindat))

regs=open(path.join(args.data, "regs.{}".format(args.index)), "r")
for line in regs:
    words = line.split()
    m = re.match("^R([0-9]+)$", words[0])
    if m:
        state.write("R GR{} 0x{}\n".format(m.group(1), words[1]))
        continue
    m = re.match("^F([0-9]+)$", words[0])
    if m:
        state.write("R FPR{} 0x{}\n".format(m.group(1), words[1]))
        continue
    m = re.match("^V([0-9]+)", words[0])
    if m:
        state.write("R VR{} 0x{}\n".format(m.group(1), words[1]))
        continue

test=open(args.test, "w")
test.write("""
[MPT]
mpt_version = 0.5

[STATE]
contents = {}

[CODE]
instructions=
""".format(args.state))

def sampled_code(addr):
    for page in codepages:
        if page <= addr <= page+args.pagesize: return True
    return False

for module in modules:
    name, begin, end = module
    print("* {}".format(name))
    test.write("; Module: {}\n".format(name))
    objdump_cmd = subprocess.Popen("objdump -d {}".format(name),
                                   shell=True, stdout=subprocess.PIPE)
    for line in objdump_cmd.stdout:
        if len(line) == 0: continue
        if line[0] != " ":
            m = re.match("^\w+\s<(.*)>:\n$", line)
            if not m: continue
            label = m.group(1)
            test.write(";  <{}>:\n".format(label))
        else:
            m = re.match("^\s+(\w*):\s((?:\w{2} )+)\s(.*)$", line)
            if not m: continue
            addr=int(m.group(1), 16)
            text=m.group(2).replace(" ", "")
            if re.match("^0+$", text): continue
            if re.match(".long", line): continue
            rest=" ".join(m.group(3).split()[:2]).replace("%","")
            if not begin <= addr <= end: addr += begin
            if args.code:
                if not sampled_code(addr): continue
            test.write("    0x{:x}: 0x{} ; {}\n".format(addr, text, rest))

