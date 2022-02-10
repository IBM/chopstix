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
import csv
import argparse
from os.path import join

from src.clustering import ClusteringInformation
from src.perfmetrics import load_invocations_from_file, aggregate_metrics, Function
from src.misc import chop_print

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Report the weight of a microbenchmark"
    )

    parser.add_argument("functions_txt")
    parser.add_argument("performance_csv")
    parser.add_argument("cluster_json")
    parser.add_argument("function_name")
    parser.add_argument("invocation", type=int)

    args = parser.parse_args()

    cluster_info = ClusteringInformation.from_file(args.cluster_json)
    invocations = load_invocations_from_file(args.performance_csv)

    weight = None

    with open(args.functions_txt) as file:
        lines = file.readlines()[1:]

        for line in lines:
            _, name, _, _, weight, _, _ = line.split("\t")

            weight = float(weight[:-1]) / 100

            if name == args.function_name:
                break

    function = Function(cluster_info, invocations, weight)
    print(function.get_weight_of_invocation(args.invocation))
