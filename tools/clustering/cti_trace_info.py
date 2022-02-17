#!/usr/bin/env python3
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 CHOPSTIX Authors
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
from src.trace import Trace
from src.misc import chop_print


def main():
    parser = argparse.ArgumentParser(description="Inspect ChopStix traces")
    parser.add_argument("trace_file")
    args = parser.parse_args()

    trace = Trace(args.trace_file)

    print("Trace parsed.")
    print("Subtrace Count:   ", trace.get_subtrace_count())
    print("Invocation Count: ", trace.get_invocation_count())
    print("Distinct Invocations Count: ", trace.get_invocation_set_count())


if __name__ == "__main__":
    main()