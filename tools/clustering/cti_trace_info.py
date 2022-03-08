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
import warnings
import numpy as np
from src.trace import Trace
from src.misc import chop_print
from src.perfmetrics import load_invocations_from_file


def main():
    parser = argparse.ArgumentParser(
        description="Inspect ChopStix traces either generated during tracing"
        " or by the chop-perf-invok support tool."
    )
    parser.add_argument(
        "-t",
        "--input_type",
        help="Input file type",
        type=str,
        default="ipc",
        choices=["trace", "ipc"],
    )
    parser.add_argument("input_file")
    args = parser.parse_args()

    chop_print("Parsing: '%s'" % args.input_file)

    if args.input_type == "trace":
        trace = Trace(args.input_file)
        print("Input file: %s" % args.input_file)
        print("Subtrace Count: %d" % trace.get_subtrace_count())
        print("Invocation Count: %d" % trace.get_invocation_count())
        print(
            "Distinct Invocations Count: %d" % trace.get_invocation_set_count()
        )
        exit(0)
    else:
        trace = load_invocations_from_file(args.input_file)

    print("Input file: %s" % args.input_file)
    print("Distinct Invocations Count: %d" % len(trace))

    metrics = []
    instr = np.array([invocation.metrics.instructions for invocation in trace])
    metrics.append(("Instructions", instr))
    ipcs = np.array([invocation.metrics.ipc for invocation in trace])
    metrics.append(("IPC", ipcs))
    cycles = np.array([invocation.metrics.cycles for invocation in trace])
    metrics.append(("Cycles", cycles))
    mem_instrs = np.array(
        [invocation.metrics.mem_instrs for invocation in trace]
    )
    metrics.append(("Memory instructions", mem_instrs))
    misses = np.array([invocation.metrics.misses for invocation in trace])
    metrics.append(("Misses", misses))

    for metric, value in metrics:
        print("Average %s per invocation: %.2f" % (metric, np.average(value)))
        print("Stdev of %s per invocation: %.2f" % (metric, np.std(value)))
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")
            print(
                "%%Stdev of %s per invocation: %.2f %%"
                % (metric, 100 * np.std(value) / np.average(value))
            )

    exit(0)


if __name__ == "__main__":
    main()
