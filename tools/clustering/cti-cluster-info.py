#!/bin/python3

import argparse
from src.clustering import ClusteringInformation

def summary(args):
    cluster = ClusteringInformation.from_file(args.cluster_file)

    print("Cluster information parsed.")
    print("Epsilon parameter: ", cluster.get_epsilon())
    print("Invocation count: %d (in %d sets)" % (cluster.get_invocation_count(), cluster.get_invocation_set_count()))
    print("Cluster count: ", cluster.get_cluster_count())
    print("Noise invocations: %d (in %d sets)" % (cluster.get_noise_invocation_count(), cluster.get_noise_invocation_set_count()))

def noise(args):
    cluster_info = ClusteringInformation.from_file(args.cluster_file)


def representative(args):
    cluster_info = ClusteringInformation.from_file(args.cluster_file)

    if args.noise:
        for invocation in cluster_info.get_random_noise_invocations():
            print(invocation)
    elif args.cluster != None:
        print(cluster_info.get_random_invocation_in_cluster(args.cluster))
    else:
        for index in range(cluster_info.get_cluster_count()):
            print(cluster_info.get_random_invocation_in_cluster(index))

        for invocation in cluster_info.get_random_noise_invocations():
            print(invocation)

def invocation(args):
    cluster_info = ClusteringInformation.from_file(args.cluster_file)

    result = cluster_info.get_cluster_id_for_invocation(args.invocation_id)
    if result == None:
        print("Invocation %d doesn't exist" % args.invocation_id)
    elif result == -1:
        print("Invocatoin %d is a noise point" % args.invocation_id)
    else:
        print("Invocation %d belongs to cluster %d" % (args.invocation_id, result))

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="Inspect cluster data")

    subparsers = parser.add_subparsers()

    parser_summary = subparsers.add_parser('summary',
            description="Give a summary of the clustering results")
    parser_summary.set_defaults(function=summary)

    parser_representative = subparsers.add_parser('representative',
            description="Provide one or more representatives of each cluster and noise points")
    parser_representative.set_defaults(function=representative)
    group = parser_representative.add_mutually_exclusive_group()
    group.add_argument('--noise', '-n', action='store_true',
            help="Only provide representatives of noise points")
    group.add_argument('--cluster', '-c', type=int,
            help="Only provide representatives of the specified cluster")

    parser_invocation = subparsers.add_parser('invocation',
            description="Provide more information of a particular invocation")
    parser_invocation.add_argument('invocation_id', type=int)
    parser_invocation.set_defaults(function=invocation)

    parser.add_argument('cluster_file')

    args = parser.parse_args()
    args.function(args)
