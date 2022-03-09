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
import os
from sklearn.cluster import DBSCAN
from sklearn.neighbors import NearestNeighbors
from dbscan1d.core import DBSCAN1D
from sklearn import metrics as skmetrics
from kneed import KneeLocator
import numpy as np
import math
import random
import warnings

random.seed(1213)

import json

from sklearn.metrics.pairwise import pairwise_distances_chunked

import datetime

import matplotlib

matplotlib.use("Agg")


from matplotlib import pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable

from src.distance import disjoint_sets, distance_2d
from src.misc import chop_print
from src.plot import instr_ipc_cluster_plot


class ClusteringInformation:

    __slots__ = (
        "_epsilon",
        "_invocation_sets",
        "_clusters",
        "_noise_invocations",
        "_extra",
    )

    def __init__(
        self, epsilon, invocation_sets, clusters, noise_invocations, extra=None
    ):
        for x in range(len(clusters)):
            x_cluster = clusters[x]
            for invocation_set in x_cluster:
                for y in range(len(clusters)):
                    if x == y:
                        continue
                    y_cluster = clusters[y]
                    assert invocation_set not in y_cluster, (
                        "Invocation Set %d is present in clusters %d and %d."
                        % (
                            invocation_set,
                            x,
                            y,
                        )
                    )

        self._epsilon = epsilon
        self._invocation_sets = invocation_sets
        self._clusters = clusters
        self._noise_invocations = noise_invocations
        self._extra = extra

    def get_epsilon(self):
        return self._epsilon

    def has_extra(self):
        return self._extra != None

    def get_extra(self):
        return self._extra

    def get_instruction_coverage(self, cluster=None):
        if not self.has_extra():
            return -1

        if cluster is not None:
            return self._extra["coverage"][str(cluster)] * 100

        return self._extra["instr_coverage"] * 100

    def get_invocation_coverage(self, cluster=None):
        if not self.has_extra():
            return -1

        if cluster is not None:
            return (
                len(self._invocation_sets[cluster])
                / self.get_invocation_count()
            ) * 100

        return self._extra["inv_coverage"] * 100

    def get_invocation_count(self, cluster=None):

        if cluster is not None:
            return len(self._invocation_sets[cluster])

        count = 0

        for invocation_set in self._invocation_sets:
            count += len(invocation_set)

        return count

    def get_extra_cluster_metric(self, cluster, metric):
        if not self.has_extra():
            return -1
        if "%s_metric" % metric not in self._extra:
            return -1

        return self._extra["%s_metric" % metric][str(cluster)]

    def get_invocation_set_count(self):
        return len(self._invocation_sets)

    def get_cluster_count(self):
        return len(self._clusters)

    def get_all_invocations_in_cluster(self, cluster):
        invocations = []

        for invocation_set in self._clusters[cluster]:
            invocations.extend(self._invocation_sets[invocation_set])

        return invocations

    def get_invocation_in_cluster(self, cluster, ignore=[]):

        if not (0 <= cluster < len(self._clusters)):
            chop_print(
                "ERROR: Wrong cluster. Valid cluster range [0,%d)"
                % len(self._clusters)
            )
            exit(1)

        if not self.has_extra():
            invocation_set = random.choice(self._clusters[cluster])
            inv = random.choice(self._invocation_sets[invocation_set])
            while inv in ignore:
                inv = random.choice(self._invocation_sets[invocation_set])
            return inv

        for index in range(0, len(self._invocation_sets[cluster])):
            inv = self._invocation_sets[cluster][index]
            if inv not in ignore:
                return inv

    def get_noise_invocation_set_count(self):
        if len(self._noise_invocations) == 0:
            return 0
        if isinstance(self._noise_invocations[0], int):
            return 1
        else:
            return len(self._noise_invocations)

    def get_noise_invocation_count(self):

        if len(self._noise_invocations) == 0:
            return 0
        if isinstance(self._noise_invocations[0], int):
            return len(self._noise_invocations)

        count = 0
        for invocation_set in self._noise_invocations:
            count += len(self._invocation_sets[invocation_set])

        return count

    # Returns a random invocation from each of the invocation sets which
    # are considered to be a noise point (i.e. don't belong to any cluster)
    def get_noise_invocations(self, ignore=[]):

        if len(self._noise_invocations) == 0:
            return None
        if isinstance(self._noise_invocations[0], int):
            inv = random.choice(self._noise_invocations)
            while inv in ignore:
                inv = random.choice(self._noise_invocations)
            return [inv]

        invocations = []

        for invocation_set in self._noise_invocations:
            invocations.append(
                random.choice(self._invocation_sets[invocation_set])
            )

        return invocations

    # Returns all the invocations of all the invocation sets which are
    # considered to be noise
    def get_all_noise_invocations(self):
        invocations = []

        for invocation_set in self._noise_invocations:
            invocations.extend(self._invocation_sets[invocation_set])

        return invocations

    def get_all_noise_invocation_sets(self):
        return self._noise_invocations

    def get_all_invocations_in_invocation_set(self, invocation_set):
        return self._invocation_sets[invocation_set]

    def get_all_invocation_sets(self):
        return self._invocation_sets

    def get_cluster_id_for_invocation(self, invocation_id):

        if not (0 <= invocation_id < self.get_invocation_count()):
            chop_print(
                "ERROR: Wrong invocation. Valid cluster range [0,%d)"
                % self.get_invocation_count()
            )
            exit(1)

        if len(self._noise_invocations) == 0:
            pass
        elif isinstance(self._noise_invocations[0], int):
            if invocation_id in self._noise_invocations:
                return -1
        else:
            for invocation_set in self._noise_invocations:
                if invocation_id in self._invocation_sets[invocation_set]:
                    return -1

        for cluster_id in range(len(self._clusters)):
            for invocation_set_id in self._clusters[cluster_id]:
                if invocation_id in self._invocation_sets[invocation_set_id]:
                    return cluster_id

        return None

    def to_file(self, path):
        json_obj = {
            "epsilon": self._epsilon,
            "extra": self._extra,
            "clusters": self._clusters,
            "noise_invocations": self._noise_invocations,
            "invocation_sets": self._invocation_sets,
        }

        with open(path, "w") as mfile:
            json.dump(json_obj, mfile, indent=2)

    @staticmethod
    def from_file(path):
        with open(path, "r") as mfile:
            json_obj = json.load(mfile)

        extra = None
        if "extra" in json_obj:
            extra = json_obj["extra"]

        return ClusteringInformation(
            json_obj["epsilon"],
            json_obj["invocation_sets"],
            json_obj["clusters"],
            json_obj["noise_invocations"],
            extra=extra,
        )


def estimate_dbscan_epsilon(trace, coverage):

    assert 0 < coverage <= 1, "Coverage should be within the (0,1] range."

    n = trace.get_invocation_count()

    chop_print("Finding eps parameter based on coverage of %f..." % coverage)
    distance_matrix = trace.get_distance_matrix(disjoint_sets)
    distances = []
    for i in range(n):
        distances.append(
            min([distance_matrix[i, j] for j in range(n) if j != i])
        )

    pointLimit = math.ceil((n - 1) * coverage)
    distances = np.sort(distances)
    uniquedist = np.unique(distances)
    distanceLimit = distances[pointLimit]
    nextDist = uniquedist[1 + np.where(uniquedist == distanceLimit)[0]]

    chop_print(
        "eps parameter based on coverage of %f set to %f"
        % (coverage, (distanceLimit + nextDist) / 2)
    )
    return (distanceLimit + nextDist) / 2


def dbscan(trace, epsilon):
    distance_matrix = trace.get_distance_matrix(disjoint_sets)

    chop_print("Clustering using parameters: eps = %f;" % epsilon)
    db = DBSCAN(metric="precomputed", eps=epsilon).fit(distance_matrix)
    core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
    core_samples_mask[db.core_sample_indices_] = True
    labels = db.labels_

    # Number of clusters in labels, ignoring noise if present.
    n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)
    n_noise_ = list(labels).count(-1)

    chop_print("Estimated number of clusters: %d" % n_clusters_)
    chop_print(
        "Estimated number of noise points: %d (%f%%)"
        % (n_noise_, n_noise_ * 100 / trace.get_invocation_set_count())
    )
    if n_clusters_ > 1:
        silhouette_score = skmetrics.silhouette_score(
            distance_matrix, labels, metric="precomputed"
        )
        chop_print("Silhouette Coefficient: %0.3f" % silhouette_score)

    clusters = [[] for i in range(n_clusters_)]
    noise_invocations = []
    for i in range(len(labels)):
        label = labels[i]

        if label == -1:
            noise_invocations.append(i)
        else:
            clusters[label].append(i)

    invocation_sets = [
        invocation_set.invocations for invocation_set in trace.invocation_sets
    ]

    return ClusteringInformation(
        epsilon, invocation_sets, clusters, noise_invocations
    )


def dbscan_ipc_instr(
    invocations,
    epsilon,
    plot_path=None,
    benchmark_name="Unk",
    function_name="Unk",
):

    data = []
    for i in range(len(invocations)):
        invocation = invocations[i]
        done = False
        for dp in data:
            if (
                not done
                and abs(1 - (invocation.metrics.instructions / dp[1])) < 0.01
            ):
                count = len(dp[0])
                dp[0].append(invocation)
                dp[1] = ((dp[1] * count) + invocation.metrics.instructions) / (
                    count + 1
                )
                dp[2] = ((dp[2] * count) + invocation.metrics.ipc) / (
                    count + 1
                )
                dp[3].append(i)
                done = True
        if done == False:
            data.append(
                [
                    [invocation],
                    invocation.metrics.instructions,
                    invocation.metrics.ipc,
                    [i],
                ]
            )

    X = np.array([[dp[2] * 20, math.log(dp[1])] for dp in data])

    db = DBSCAN(eps=epsilon, algorithm="kd_tree").fit(X)
    core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
    core_samples_mask[db.core_sample_indices_] = True
    labels = db.labels_

    cluster_count = len(set(labels))

    if -1 in labels:
        cluster_count -= 1  # -1 represents noise points, not clusters

    clusters = [[] for i in range(cluster_count)]
    invocation_sets = [dp[3] for dp in data]
    noise_invocations = []
    for i in range(len(labels)):
        label = labels[i]

        if label == -1:
            noise_invocations.append(i)
        else:
            clusters[label].append(i)

    ipcs = np.array([invocation.metrics.ipc for invocation in invocations])
    instr = np.array(
        [invocation.metrics.instructions for invocation in invocations]
    )
    all_instr = sum(instr)

    (
        instr_coverage,
        inv_coverage,
        coverage,
        centroids,
        metrics,
    ) = clustering_evaluation(
        invocations,
        instr,
        invocation_sets,
        noise_invocations,
        all_instr,
        ipcs=ipcs,
    )

    instr_ipc_cluster_plot(
        plot_path,
        instr,
        ipcs,
        labels,
        centroids,
        instr_coverage,
        inv_coverage,
        benchmark_name=benchmark_name,
        function_name=function_name,
        method="DBSCAN IPC/Instr",
    )

    extra_info = {}
    extra_info["instr_coverage"] = instr_coverage
    extra_info["inv_coverage"] = inv_coverage
    extra_info["coverage"] = coverage
    extra_info["centroids"] = centroids
    extra_info["instructions_metric"] = metrics["instructions"]
    extra_info["cycles_metric"] = metrics["cycles"]
    extra_info["ipc_metric"] = metrics["ipc"]

    return ClusteringInformation(
        epsilon, invocation_sets, clusters, noise_invocations, extra=extra_info
    )


def dbscan_ipc(
    invocations,
    epsilon,
    plot_path=None,
    benchmark_name="Unk",
    function_name="Unk",
):
    # init dbscan object
    dbs = DBSCAN1D(eps=epsilon)

    ipcs = np.array([invocation.metrics.ipc for invocation in invocations])
    instr = np.array(
        [invocation.metrics.instructions for invocation in invocations]
    )
    all_instr = sum(instr)

    # get labels for each point
    labels = dbs.fit_predict(ipcs)
    cluster_count = len(set(labels))

    if -1 in labels:
        cluster_count -= 1  # -1 represents noise points, not clusters

    clusters = [[i] for i in range(cluster_count)]
    invocation_sets = [[] for i in range(cluster_count)]
    noise_invocations = []
    for i in range(len(labels)):
        label = labels[i]

        if label == -1:
            noise_invocations.append(i)
        else:
            invocation_sets[label].append(i)

    (
        instr_coverage,
        inv_coverage,
        coverage,
        centroids,
        metrics,
    ) = clustering_evaluation(
        invocations,
        instr,
        invocation_sets,
        noise_invocations,
        all_instr,
        ipcs=ipcs,
    )

    instr_ipc_cluster_plot(
        plot_path,
        instr,
        ipcs,
        labels,
        centroids,
        instr_coverage,
        inv_coverage,
        benchmark_name=benchmark_name,
        function_name=function_name,
        method="DBSCAN IPC",
    )

    extra_info = {}
    extra_info["instr_coverage"] = instr_coverage
    extra_info["inv_coverage"] = inv_coverage
    extra_info["coverage"] = coverage
    extra_info["centroids"] = centroids
    extra_info["instructions_metric"] = metrics["instructions"]
    extra_info["cycles_metric"] = metrics["cycles"]
    extra_info["ipc_metric"] = metrics["ipc"]

    return ClusteringInformation(
        epsilon, invocation_sets, clusters, noise_invocations, extra=extra_info
    )


def dbscan_instr(
    invocations,
    epsilon=None,
    raw=False,
    msamples=None,
    rec_level=0,
    plot_path=None,
    fplot=True,
    uniq_value_threshold=50,
    minimum_distance_percentage=0.1,
    maximum_distance_percentage=5,
    minimum_cluster_size_percentage=1,
    minimum_cluster_count=10,
    maximum_cluster_count=50,
    benchmark_name="Unk",
    function_name="Unk",
):

    #
    # This algorithm assumes no outliers are present in the input data set.
    # It usses DBSCAN as the clustering algorithm, and uses some heuristics
    # and recursive greedy search to derive the minsamples as well as the
    # epsilon parameters.
    #

    if not raw:
        epsilon = None

    one_dim = True
    enable_weighted = False
    if one_dim:
        # TODO: At the time of the implementation, DBSCAN1D does not support
        # weighted parameter. Enable once suport is implemented.
        enable_weighted = False

    # Get data
    if not raw:
        instr = np.array(
            [invocation.metrics.instructions for invocation in invocations]
        )
        ipcs = np.array([invocation.metrics.ipc for invocation in invocations])
    else:
        instr = np.array(invocations)

    all_instr = sum(instr)
    assert len(instr) > 0, "Not samples?"
    assert min(instr) >= 0

    # Scale data to [0, 1] range
    # (will facilitate epsilon comparisons between different datasets)
    scaled_instr = instr / max(instr)

    # Convert to a 2D np array
    scaled_2D = np.vstack((scaled_instr, scaled_instr)).T

    #
    # Heuristic, if the range of unique value is very small, in absolute
    # terms simply set a very small epsilon to define that many clusters.
    #
    uniq_values = len(set(instr.flatten()))
    if uniq_values < uniq_value_threshold:
        epsilon = 0.00001

    #
    # Try to compute an optimal epsilon
    #
    # REFERENCE:
    #
    # Rahmah, Nadia & Sitanggang, Imas. (2016). Determination of Optimal
    # Epsilon (Eps) Value on DBSCAN Algorithm to Clustering Data on Peatland
    # Hotspots in Sumatra. IOP Conference Series: Earth and Environmental
    # Science. 31. 012012. 10.1088/1755-1315/31/1/012012.
    #

    distances = [0]
    if epsilon is None:
        if one_dim:
            nndata = np.sort(scaled_instr)
            distances = np.zeros(len(nndata))
            distances[0] = nndata[1] - nndata[0]
            distances[1:-1] = [
                min(nndata[x + 1] - nndata[x], nndata[x] - nndata[x - 1])
                for x in range(1, len(nndata) - 1)
            ]
            distances[-1] = nndata[-1] - nndata[-2]
            distances = np.sort(distances)
        else:
            nndata = scaled_2D
            neigh = NearestNeighbors(n_neighbors=2, algorithm="kd_tree")
            nbrs = neigh.fit(nndata)
            distances, indices = nbrs.kneighbors(nndata)
            distances = np.sort(distances, axis=0)
            distances = distances[:, 1]

        #
        # Filter out very small variations
        #
        for idx in range(len(distances)):
            if distances[idx] < (minimum_distance_percentage / 100):
                distances[idx] = 0

    #
    # Maximum distance
    #
    percent = maximum_distance_percentage / 100
    if one_dim:
        threshold = percent
    else:
        threshold = distance_2d(percent, percent)

    if epsilon is not None:
        pass
    else:
        #
        # Find the knee of the distance curve
        #
        # REFERENCE:
        #
        # Finding a “Kneedle” in a Haystack: Detecting Knee Points in System
        # Behavior Ville Satopa † , Jeannie Albrecht† , David Irwin‡ , and
        # Barath Raghavan§ †Williams College, Williamstown, MA ‡University of
        # Massachusetts Amherst, Amherst, MA § International Computer Science
        # Institute, Berkeley, CA
        #

        #
        # Optimize KneeLocator by removing zeroes
        #
        while distances[int(len(distances) * 0.90)] == 0:
            distances = distances[int(len(distances) * 0.10) :]

        #
        # Optimize execution time for large data sets
        #
        if len(distances) > 100000:
            nzero = distances.nonzero()[0]
            displ = nzero[0]
            distances = distances[int(displ) * 0.8 :]

        # TODO: distance plot
        # plt.plot(distances)
        # plt.show()

        s = max(int(len(distances) * 0.1), 1)

        kl = KneeLocator(
            np.arange(0, len(distances)),
            distances,
            curve="convex",
            direction="increasing",
            online=True,
            S=s,
        )

        # Assume a symmetric/round knee
        knee = kl.knee + int(((len(distances) - kl.knee) / 2)) + 1
        if knee >= len(distances):
            knee = len(distances) - 1

        epsilon = distances[knee]

    #
    # Do the clustering
    #
    # REFERENCE:
    #
    #   “A Density-Based Algorithm for Discovering Clusters in Large Spatial
    #   Databases with Noise” Ester, M., H. P. Kriegel, J. Sander, and X. Xu,
    #   In Proceedings of the 2nd International Conference on Knowledge
    #   Discovery and Data Mining, Portland, OR, AAAI Press, pp. 226–231. 1996
    #
    #   “DBSCAN revisited, revisited: why and how you should (still) use
    #   DBSCAN. Schubert, E., Sander, J., Ester, M., Kriegel, H. P., & Xu,
    #   X. (2017). In ACM Transactions on Database Systems (TODS), 42(3),
    #   19.
    #

    # A cluster should be defined if at least it contains a X % of the samples
    if msamples is None:
        msamples = max(
            int(len(instr) * (minimum_cluster_size_percentage / 100)), 1
        )

    if enable_weighted:
        # Compute weights according to the number of instructions and msamples
        # There are invocations that are sufficiently large to be considered a
        # non noise (core sample) regardless of the msamples around them.
        #
        # If a sample contains more than 5% of the total instructions, is
        # considered a core sample
        #
        # TODO: Expose the 5% as a parameter
        weights = (instr / (all_instr * 0.05)) * msamples

    if not one_dim:
        db = DBSCAN(eps=epsilon, min_samples=msamples, algorithm="kd_tree")
        if enable_weighted:
            db.fit(scaled_2D, None, weights)
        else:
            db.fit(scaled_2D)
        labels = db.labels_
    else:
        db = DBSCAN1D(eps=epsilon, min_samples=msamples)
        if enable_weighted:
            labels = db.fit_predict(scaled_2D[:, 0], None, weights)
        else:
            labels = db.fit_predict(scaled_2D[:, 0])

    # colors = ['royalblue', 'maroon', 'forestgreen', 'mediumorchid', 'tan', 'deeppink', 'olive', 'goldenrod', 'lightcyan', 'navy']
    # vectorizer = np.vectorize(lambda x: colors[x % len(colors)])
    # plt.scatter(scaled_2D[:,0], scaled_2D[:,1], c=vectorizer(labels))
    # plt.show()

    # Compute invocation information
    cluster_count = len(set(labels))
    if -1 in labels:
        cluster_count -= 1  # -1 represents noise points, not clusters

    clusters = [[i] for i in range(cluster_count)]
    invocation_sets = [[] for i in range(cluster_count)]
    noise_invocations = []
    for i in range(len(labels)):
        label = labels[i]
        if label == -1:
            noise_invocations.append(i)
        else:
            invocation_sets[label].append(i)

    assert cluster_count == len(invocation_sets)

    #
    # Evaluate and call recursivelly if solution does not look good
    #
    if cluster_count < minimum_cluster_count:
        if (
            len(noise_invocations) < 10
            and cluster_count == 1
            and rec_level < 10
        ):
            # All in the same cluster? Reduce epsilon and msamples
            if fplot == True:
                return dbscan_instr(
                    invocations,
                    epsilon=epsilon / 10,
                    msamples=msamples / 10,
                    rec_level=rec_level + 1,
                    fplot=fplot,
                    plot_path=plot_path,
                    uniq_value_threshold=uniq_value_threshold,
                    minimum_distance_percentage=minimum_distance_percentage,
                    maximum_distance_percentage=maximum_distance_percentage,
                    minimum_cluster_size_percentage=minimum_cluster_size_percentage,
                    minimum_cluster_count=minimum_cluster_count,
                    maximum_cluster_count=maximum_cluster_count,
                    benchmark_name=benchmark_name,
                    function_name=function_name,
                )
            else:
                return dbscan_instr(
                    instr,
                    epsilon=epsilon / 10,
                    raw=True,
                    msamples=msamples / 10,
                    rec_level=rec_level + 1,
                    fplot=fplot,
                    plot_path=plot_path,
                    uniq_value_threshold=uniq_value_threshold,
                    minimum_distance_percentage=minimum_distance_percentage,
                    maximum_distance_percentage=maximum_distance_percentage,
                    minimum_cluster_size_percentage=minimum_cluster_size_percentage,
                    minimum_cluster_count=minimum_cluster_count,
                    maximum_cluster_count=maximum_cluster_count,
                    benchmark_name=benchmark_name,
                    function_name=function_name,
                )

        if len(noise_invocations) < (maximum_cluster_count - cluster_count):
            # Take all noise and convert to clusters
            for noise_invocation in noise_invocations:
                noise_invocations.remove(noise_invocation)
                invocation_sets.append([])
                invocation_sets[-1].append(noise_invocation)
                clusters.append([cluster_count])
                cluster_count = cluster_count + 1
                assert cluster_count == len(invocation_sets)

        elif rec_level < 10:
            # Reaply DBSCAN on the noise
            values = [instr[y] for y in noise_invocations]
            invmap = [(x, y) for x, y in enumerate(noise_invocations)]
            invmap = dict(invmap)
            cinfo = dbscan_instr(
                [val for val in values],
                epsilon=None,
                raw=True,
                rec_level=rec_level + 1,
                fplot=False,
                plot_path=plot_path,
                uniq_value_threshold=uniq_value_threshold,
                minimum_distance_percentage=minimum_distance_percentage,
                maximum_distance_percentage=maximum_distance_percentage,
                minimum_cluster_size_percentage=minimum_cluster_size_percentage,
                minimum_cluster_count=minimum_cluster_count,
                maximum_cluster_count=maximum_cluster_count,
                benchmark_name=benchmark_name,
                function_name=function_name,
            )
            new_invocations_sets = cinfo.get_all_invocation_sets()

            for nset in new_invocations_sets:
                new_set = []
                for ninvocation in nset:
                    new_set.append(invmap[ninvocation])
                    noise_invocations.remove(invmap[ninvocation])
                invocation_sets.append(new_set)
                clusters.append([cluster_count])
                cluster_count = cluster_count + 1
                assert cluster_count == len(invocation_sets)

    (
        instr_coverage,
        inv_coverage,
        coverage,
        centroids,
        metrics,
    ) = clustering_evaluation(
        invocations, instr, invocation_sets, noise_invocations, all_instr
    )

    extra_info = {}
    extra_info["instr_coverage"] = instr_coverage
    extra_info["inv_coverage"] = inv_coverage
    extra_info["coverage"] = coverage
    extra_info["centroids"] = centroids
    extra_info["instructions_metric"] = metrics["instructions"]
    extra_info["cycles_metric"] = metrics["cycles"]
    extra_info["ipc_metric"] = metrics["ipc"]

    if fplot == True:
        for label, indexes in zip(clusters, invocation_sets):
            for index in indexes:
                labels[index] = label[0]

        for index in noise_invocations:
            labels[index] = -1

        instr_ipc_cluster_plot(
            plot_path,
            instr,
            ipcs,
            labels,
            centroids,
            instr_coverage,
            inv_coverage,
            benchmark_name=benchmark_name,
            function_name=function_name,
            method="DBSCAN Instr",
        )

    assert len(invocation_sets) == len(clusters)
    assert isinstance(invocation_sets, list)
    assert isinstance(clusters, list)
    assert isinstance(noise_invocations, list)
    return ClusteringInformation(
        epsilon, invocation_sets, clusters, noise_invocations, extra=extra_info
    )


def brute_force_2d_density(
    invocations,
    epsilon=None,
    plot_path=None,
    max_clusters=20,
    min_clusters_weight_percentage=1,
    target_coverage_percentage=90,
    outlier_percent=1,
    outlier_minsize_threshold=1000,
    minimum_granularity_percentage=1,
    granularity_step_percentage=1,
    benchmark_name="Unk",
    function_name="Unk",
):

    instr = np.array(
        [invocation.metrics.instructions for invocation in invocations]
    )
    ipcs = np.array([invocation.metrics.ipc for invocation in invocations])

    if len(instr) == 0:
        chop_print("Input trace is empty. Exiting...")
        exit(0)

    if min(instr) == 0:
        chop_print(
            "Input trace contains samples with 0 instructions. Exiting..."
        )
        exit(0)

    # Remove the outliers (X% top and bottom data points),
    # only if data # points above a threshold are provided.
    hist_range = [[min(instr), max(instr)], [min(ipcs), max(ipcs)]]
    all_instr = sum(instr)
    if len(instr) > outlier_minsize_threshold:
        idx = int(len(instr) * (outlier_percent / 100))
        hist_range = [
            [min(sorted(instr)[idx:]) * 0.9, max(sorted(instr)[0:-idx]) * 1.1],
            [min(sorted(ipcs)[idx:]) * 0.9, max(sorted(ipcs)[0:-idx]) * 1.1],
        ]
        all_instr = sum(sorted(instr)[idx:-idx])

    weights = instr / all_instr

    assert min(instr) >= 0
    assert min(ipcs) >= 0

    coverage = 0
    step = int(granularity_step_percentage / minimum_granularity_percentage)
    if step < 1:
        step = 1
    cbins = int(100 / minimum_granularity_percentage) + step

    while coverage < (target_coverage_percentage / 100):
        cbins = max(cbins - step, 1)
        hist, xedges, yedges = np.histogram2d(
            instr, ipcs, bins=cbins, weights=weights, range=hist_range
        )

        coverage = np.sort(hist.flatten())
        if len(coverage) < max_clusters:
            coverage = 1
        else:
            coverage = np.sum(coverage[-max_clusters:])

    xranges = [[xedges[x], xedges[x + 1]] for x in range(0, cbins)]
    xranges[-1][1] = xranges[-1][1] * 1.01
    yranges = [[yedges[x], yedges[x + 1]] for x in range(0, cbins)]
    yranges[-1][1] = yranges[-1][1] * 1.01

    x = np.repeat(np.arange(cbins), cbins)
    y = np.tile(np.arange(cbins), cbins)
    combined = np.array(list(zip(hist.flatten(), x, y)))
    combined = sorted(combined, key=lambda x: x[0])

    #
    # Cluters with at least X % of the sampled instructions are considered
    #
    combined = [
        xy
        for xy in combined
        if xy[0] >= (min_clusters_weight_percentage / 100)
    ]

    if len(combined) > max_clusters:
        combined = combined[-max_clusters:]

    combined = list(reversed(combined))

    topxidx = [int(xy[1]) for xy in combined]
    topyidx = [int(xy[2]) for xy in combined]

    # assign labels to each x,y
    labeldict = {}
    clabel = 0
    clusters = []
    for xy in zip(topxidx, topyidx):
        assert xy not in labeldict
        labeldict[xy] = clabel
        clusters.append([clabel])
        clabel = clabel + 1

    assert clabel == len(clusters)
    assert clabel == len(labeldict.keys())

    divx = xedges[1] - xedges[0]
    divy = yedges[1] - yedges[0]

    # Assign labels to data points
    labels = []
    noise_invocations = []
    invocation_sets = [[] for cl in range(clabel)]

    assert len(invocation_sets) == len(clusters)

    for sample in range(0, len(ipcs)):
        # This is possible because all bins are equal
        xidx = int((instr[sample] - xedges[0]) / divx)
        yidx = int((ipcs[sample] - yedges[0]) / divy)

        #
        # Handle last bin case
        #
        if xidx == cbins:
            xidx = xidx - 1
        if yidx == cbins:
            yidx = yidx - 1

        # Validate the right bin
        if xidx >= cbins or xidx < 0:
            # outlier
            pass
        elif not xranges[xidx][0] <= instr[sample] < xranges[xidx][1]:
            if instr[sample] >= xranges[xidx][1]:
                xidx = xidx + 1
            else:
                xidx = xidx - 1
            if 0 <= xidx < len(xranges):
                assert xranges[xidx][0] <= instr[sample] < xranges[xidx][1]

        if yidx >= cbins or yidx < 0:
            # outlier
            pass
        elif not yranges[yidx][0] <= ipcs[sample] < yranges[yidx][1]:
            if ipcs[sample] >= yranges[yidx][1]:
                yidx = yidx + 1
            else:
                yidx = yidx - 1
            if 0 <= yidx < len(yranges):
                assert yranges[yidx][0] <= ipcs[sample] < yranges[yidx][1]

        labels.append(labeldict.get((xidx, yidx), -1))
        if labels[-1] == -1:
            noise_invocations.append(sample)
        else:
            invocation_sets[labels[-1]].append(sample)

    for label in range(len(clusters)):
        assert label in labels, label

    (
        instr_coverage,
        inv_coverage,
        coverage,
        centroids,
        metrics,
    ) = clustering_evaluation(
        invocations,
        instr,
        invocation_sets,
        noise_invocations,
        all_instr,
        ipcs=ipcs,
        custom_range=hist_range[0],
    )

    instr_ipc_cluster_plot(
        plot_path,
        instr,
        ipcs,
        labels,
        centroids,
        instr_coverage,
        inv_coverage,
        custom_range=hist_range,
        benchmark_name=benchmark_name,
        function_name=function_name,
        method="2D Density",
    )

    extra_info = {}
    extra_info["instr_coverage"] = instr_coverage
    extra_info["inv_coverage"] = inv_coverage
    extra_info["coverage"] = coverage
    extra_info["centroids"] = centroids
    extra_info["instructions_metric"] = metrics["instructions"]
    extra_info["cycles_metric"] = metrics["cycles"]
    extra_info["ipc_metric"] = metrics["ipc"]

    return ClusteringInformation(
        epsilon, invocation_sets, clusters, noise_invocations, extra=extra_info
    )


def clustering_evaluation(
    invocations,
    instr,
    invocation_sets,
    noise_invocations,
    all_instr,
    ipcs=None,
    custom_range=None,
):
    # Evaluate the percentage of instructions in a cluster as well as the
    # percentage of non noise invocations. These two metrics can be used to
    # assess the quality of the clustering. Also, select the cluster
    # representatives as the "centroid", according to geometric mean depending
    # on the dimensions given.
    coverage = {}
    centroids = {}
    metrics = {}
    mnames = ["instructions", "cycles", "ipc"]

    noise_invocations.sort(key=lambda x: instr[x], reverse=True)

    if custom_range is None:
        coverage[-1] = sum(instr[i] for i in noise_invocations) / all_instr
        for name in mnames:
            if name not in metrics:
                metrics[name] = {}
            with warnings.catch_warnings():
                warnings.simplefilter("ignore", category=RuntimeWarning)
                metrics[name][-1] = np.average(
                    np.array(
                        [
                            getattr(invocations[i].metrics, name)
                            for i in noise_invocations
                        ]
                    )
                )
            if np.isnan(metrics[name][-1]):
                metrics[name][-1] = -1
    else:
        coverage[-1] = (
            sum(
                [
                    instr[i]
                    for i in noise_invocations
                    if instr[i] >= custom_range[0]
                    and instr[i] < custom_range[1]
                ]
            )
            / all_instr
        )
        for name in mnames:
            if name not in metrics:
                metrics[name] = {}
            with warnings.catch_warnings():
                warnings.simplefilter("ignore", category=RuntimeWarning)
                metrics[name][-1] = np.average(
                    np.array(
                        [
                            getattr(invocations[i].metrics, name)
                            for i in noise_invocations
                            if instr[i] >= custom_range[0]
                            and instr[i] < custom_range[1]
                        ]
                    )
                )
            if np.isnan(metrics[name][-1]):
                metrics[name][-1] = -1

    for label in range(len(invocation_sets)):
        coverage[label] = (
            sum([instr[i] for i in invocation_sets[label]]) / all_instr
        )

        assert len(invocation_sets[label]) > 0
        gmean = np.exp(
            np.log([instr[i] for i in invocation_sets[label]]).mean()
        )
        if ipcs is not None:
            gmean2 = np.exp(
                np.log([ipcs[i] for i in invocation_sets[label]]).mean()
            )

        def diff_func(x):
            idiff = abs(instr[x] - gmean)
            if ipcs is not None:
                idiff2 = abs(ipcs[x] - gmean2)
                idiff = distance_2d(idiff, idiff2)
            return idiff

        invocation_sets[label].sort(key=lambda x: diff_func(x))
        centroids[label] = invocation_sets[label][0]

        for name in mnames:
            if name not in metrics:
                metrics[name] = {}
            metrics[name][label] = np.average(
                np.array(
                    [
                        getattr(invocations[i].metrics, name)
                        for i in invocation_sets[label]
                    ]
                )
            )

    # Coverage in instr
    instr_coverage = 1 - coverage[-1]

    # Coverage in % invocations
    if custom_range is None:
        inv_coverage = 1 - (len(noise_invocations) / len(instr))
    else:
        inv_coverage = 1 - (
            len(
                [
                    x
                    for x in noise_invocations
                    if instr[x] >= custom_range[0]
                    and instr[x] < custom_range[1]
                ]
            )
            / max(
                min(
                    len(
                        [
                            x
                            for x in instr
                            if x >= custom_range[0] and x < custom_range[1]
                        ]
                    ),
                    len(instr),
                ),
                1,
            )
        )

    return instr_coverage, inv_coverage, coverage, centroids, metrics
