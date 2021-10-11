#!/usr/bin/env python3

import csv
import argparse
from os.path import join

from src.clustering import ClusteringInformation
from src.perfmetrics import load_invocations_from_file, aggregate_metrics, Function

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Report the weight of a microbenchmark")

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
            _, name, _, _, weight, _, _ = line.split('\t')

            weight = float(weight[:-1]) / 100

            if name == args.function_name:
                break

    function = Function(cluster_info, invocations, weight)
    print(function.get_weight_of_invocation(args.invocation))

