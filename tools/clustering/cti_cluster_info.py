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
from src.clustering import ClusteringInformation
from src.misc import chop_print


def summary(args):
    cluster = ClusteringInformation.from_file(args.cluster_file)
    chop_print("Cluster information parsed.")

    if args.cluster is None:
        print("Epsilon parameter: %s" % cluster.get_epsilon())
        print(
            "Invocation count: %d (in %d sets)"
            % (
                cluster.get_invocation_count(),
                cluster.get_invocation_set_count(),
            )
        )
        print("Cluster count: %d" % cluster.get_cluster_count())
        print(
            "Noise invocations: %d (in %d sets)"
            % (
                cluster.get_noise_invocation_count(),
                cluster.get_noise_invocation_set_count(),
            )
        )

        print(
            "Instruction coverage: %.2f %%"
            % cluster.get_instruction_coverage()
        )
        print(
            "Invocation coverage: %.2f %%" % cluster.get_invocation_coverage()
        )
        return

    if not (0 <= args.cluster < cluster.get_cluster_count()):
        chop_print(
            "ERROR: Wrong cluster. Valid cluster range [0,%d)"
            % cluster.get_cluster_count()
        )
        exit(1)

    print("Cluster id: %d" % args.cluster)
    print(
        "Invocation count: %d "
        % cluster.get_invocation_count(cluster=args.cluster)
    )
    print(
        "Instruction coverage: %.2f %%"
        % cluster.get_instruction_coverage(cluster=args.cluster)
    )
    print(
        "Invocation coverage: %.2f %%"
        % cluster.get_invocation_coverage(cluster=args.cluster)
    )
    print(
        "Instructions: %d"
        % cluster.get_extra_cluster_metric(args.cluster, "instructions")
    )
    print(
        "Cycles: %d" % cluster.get_extra_cluster_metric(args.cluster, "cycles")
    )
    print("IPC: %.2f " % cluster.get_extra_cluster_metric(args.cluster, "ipc"))


def noise(args):
    cluster_info = ClusteringInformation.from_file(args.cluster_file)


def representative(args):
    cluster_info = ClusteringInformation.from_file(args.cluster_file)

    if args.noise:
        try:
            for invocation in cluster_info.get_noise_invocations(
                ignore=args.ignore
            ):
                print(invocation)
        except TypeError:
            return
    elif args.cluster != None:
        print(
            cluster_info.get_invocation_in_cluster(
                args.cluster, ignore=args.ignore
            )
        )
    else:
        for index in range(cluster_info.get_cluster_count()):
            print(
                cluster_info.get_invocation_in_cluster(
                    index, ignore=args.ignore
                )
            )
        for invocation in cluster_info.get_noise_invocations(
            ignore=args.ignore
        ):
            print(invocation)


def invocation(args):
    cluster_info = ClusteringInformation.from_file(args.cluster_file)

    result = cluster_info.get_cluster_id_for_invocation(args.invocation_id)
    if result == None:
        print("Invocation %d doesn't exist" % args.invocation_id)
    elif result == -1:
        print("Invocatoin %d is a noise point" % args.invocation_id)
    else:
        print(
            "Invocation %d belongs to cluster %d"
            % (args.invocation_id, result)
        )


def main():
    parser = argparse.ArgumentParser(description="Inspect cluster data")

    subparsers = parser.add_subparsers(dest="command")
    subparsers.required = True

    parser_summary = subparsers.add_parser(
        "summary", description="Give a summary of the clustering results"
    )
    parser_summary.set_defaults(function=summary)
    parser_summary.add_argument(
        "--cluster",
        "-c",
        type=int,
        help="Only provide information of the specified cluster",
        default=None,
    )

    parser_representative = subparsers.add_parser(
        "representative",
        description="Provide one or more representatives of each cluster and noise points",
    )
    parser_representative.set_defaults(function=representative)
    group = parser_representative.add_mutually_exclusive_group()
    group.add_argument(
        "--noise",
        "-n",
        action="store_true",
        help="Only provide representatives of noise points",
    )
    group.add_argument(
        "--cluster",
        "-c",
        type=int,
        help="Only provide representatives of the specified cluster",
        default=None,
    )
    parser_representative.add_argument(
        "--ignore",
        "-i",
        type=int,
        help="Ignore the invocations provided in the selection process",
        default=[],
        nargs="*",
    )

    parser_invocation = subparsers.add_parser(
        "invocation",
        description="Provide more information of a particular invocation",
    )
    parser_invocation.add_argument("invocation_id", type=int)
    parser_invocation.set_defaults(function=invocation)

    parser.add_argument("cluster_file")

    args = parser.parse_args()
    args.function(args)


if __name__ == "__main__":
    main()
