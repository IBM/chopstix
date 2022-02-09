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
from sklearn import metrics
from kneed import KneeLocator
import numpy as np
import math
import random
import json

from sklearn.metrics.pairwise import pairwise_distances_chunked

import datetime

import matplotlib

matplotlib.use("Agg")

from matplotlib import pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable

from src.distance import disjoint_sets


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
                    assert (
                        invocation_set not in y_cluster
                    ), "Invocation Set %d is present in clusters %d and %d." % (
                        invocation_set,
                        x,
                        y,
                    )

        self._epsilon = epsilon
        self._invocation_sets = invocation_sets
        self._clusters = clusters
        self._noise_invocations = noise_invocations
        self._extra = extra

    def get_epsilon(self):
        return self._epsilon

    def get_invocation_count(self):
        count = 0

        for invocation_set in self._invocation_sets:
            count += len(invocation_set)

        return count

    def get_invocation_set_count(self):
        return len(self._invocation_sets)

    def get_cluster_count(self):
        return len(self._clusters)

    def get_all_invocations_in_cluster(self, cluster):
        invocations = []

        for invocation_set in self._clusters[cluster]:
            invocations.extend(self._invocation_sets[invocation_set])

        return invocations

    def get_random_invocation_in_cluster(self, cluster):
        invocation_set = random.choice(self._clusters[cluster])
        return random.choice(self._invocation_sets[invocation_set])

    def get_noise_invocation_set_count(self):
        return len(self._noise_invocations)

    def get_noise_invocation_count(self):
        count = 0

        for invocation_set in self._noise_invocations:
            count += len(self._invocation_sets[invocation_set])

        return count

    # Returns a random invocation from each of the invocation sets which
    # are considered to be a noise point (i.e. don't belong to any cluster)
    def get_random_noise_invocations(self):
        invocations = []

        for invocation_set in self._noise_invocations:
            invocations.append(random.choice(self._invocation_sets[invocation_set]))

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
            "invocation_sets": self._invocation_sets,
            "clusters": self._clusters,
            "noise_invocations": self._noise_invocations,
            "extra": self._extra,
        }

        with open(path, "w") as file:
            json.dump(json_obj, file)

    @staticmethod
    def from_file(path):
        with open(path, "r") as file:
            json_obj = json.load(file)

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

    print("Finding eps parameter based on coverage of %f..." % coverage)
    distance_matrix = trace.get_distance_matrix(disjoint_sets)
    distances = []
    for i in range(n):
        distances.append(min([distance_matrix[i, j] for j in range(n) if j != i]))

    pointLimit = math.ceil((n - 1) * coverage)
    distances = np.sort(distances)
    uniquedist = np.unique(distances)
    distanceLimit = distances[pointLimit]
    nextDist = uniquedist[1 + np.where(uniquedist == distanceLimit)[0]]

    print(
        "eps parameter based on coverage of %f set to %f"
        % (coverage, (distanceLimit + nextDist) / 2)
    )
    return (distanceLimit + nextDist) / 2


def dbscan(trace, epsilon):
    distance_matrix = trace.get_distance_matrix(disjoint_sets)

    print("Clustering using parameters: eps = %f;" % epsilon)
    db = DBSCAN(metric="precomputed", eps=epsilon).fit(distance_matrix)
    core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
    core_samples_mask[db.core_sample_indices_] = True
    labels = db.labels_

    # Number of clusters in labels, ignoring noise if present.
    n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)
    n_noise_ = list(labels).count(-1)

    print("Estimated number of clusters: %d" % n_clusters_)
    print(
        "Estimated number of noise points: %d (%f%%)"
        % (n_noise_, n_noise_ * 100 / trace.get_invocation_set_count())
    )
    if n_clusters_ > 1:
        silhouette_score = metrics.silhouette_score(
            distance_matrix, labels, metric="precomputed"
        )
        print("Silhouette Coefficient: %0.3f" % silhouette_score)

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

    return ClusteringInformation(epsilon, invocation_sets, clusters, noise_invocations)


def dbscan_ipc_instr(invocations, epsilon):
    data = []
    for i in range(len(invocations)):
        invocation = invocations[i]
        done = False
        for dp in data:
            if not done and abs(1 - (invocation.metrics.instructions / dp[1])) < 0.01:
                count = len(dp[0])
                dp[0].append(invocation)
                dp[1] = ((dp[1] * count) + invocation.metrics.instructions) / (
                    count + 1
                )
                dp[2] = ((dp[2] * count) + invocation.metrics.ipc) / (count + 1)
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

    return ClusteringInformation(epsilon, invocation_sets, clusters, noise_invocations)


def dbscan_ipc(invocations, epsilon):
    # init dbscan object
    dbs = DBSCAN1D(eps=epsilon)

    ipcs = np.array([invocation.metrics.ipc for invocation in invocations])

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

    return ClusteringInformation(epsilon, invocation_sets, clusters, noise_invocations)


def dbscan_instr(
    invocations,
    epsilon,
    raw=False,
    msamples=None,
    rec_level=0,
    plotname="plot",
    fplot=True,
):

    # This algorithm assumes not outliers present in the input data set.
    # To ensure that the input trace should be noise free.

    if not raw:
        epsilon = None

    one_dim = True
    enable_weighted = False
    if one_dim:
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
    print("scale", str(datetime.datetime.now()))
    scaled_instr = instr / max(instr)

    # Convert to a 2D np array
    print("2d", str(datetime.datetime.now()))
    scaled_2D = np.vstack((scaled_instr, scaled_instr)).T

    # Heuristic, if the range of unique value is very small, in absolute
    # terms simply set a very small epsilon to define that many
    # cluster.
    #
    # Set to 50
    #
    uniq_values = len(set(instr.flatten()))
    print("samples", len(instr))
    if uniq_values < 50:
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
            print("1d neig", str(datetime.datetime.now()))
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
            print("fit neig 1", str(datetime.datetime.now()))
            nbrs = neigh.fit(nndata)
            print("fit neig 2", str(datetime.datetime.now()))
            distances, indices = nbrs.kneighbors(nndata)
            print("sort", str(datetime.datetime.now()))
            distances = np.sort(distances, axis=0)
            distances = distances[:, 1]

        #
        # Filter out very small variations <0.1%
        #
        for idx in range(len(distances)):
            if distances[idx] < 0.001:
                distances[idx] = 0

    #
    # With euclidean distance, assume a 5% variability
    #
    percent = 5
    if one_dim:
        threshold = percent / 100
    else:
        threshold = math.sqrt(2 * ((percent / 100) ** 2))

    print(max(distances))
    print(threshold)
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

        if False:
            # Assume 20% of stability. I.e. points within very small distance
            s = max(int(len(distances) * 0.2), 1)

            print("knee", s, str(datetime.datetime.now()))
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
            print("good", s, kl.knee, knee, epsilon)

            nzero = distances.nonzero()[0]
            displ = nzero[0]
            if displ > s:
                displ = displ - s

            print("knee", s, displ, str(datetime.datetime.now()))
            kl = KneeLocator(
                np.arange(0, len(distances[displ:])),
                distances[displ:],
                curve="convex",
                direction="increasing",
                online=True,
                S=s,
            )

            # Assume a symmetric/round knee
            knee = kl.knee + int(((len(distances[displ:]) - kl.knee) / 2)) + 1
            if knee >= len(distances):
                knee = len(distances) - 1

            epsilon = distances[displ:][knee]
            print("new", s, kl.knee + displ, knee + displ, epsilon)

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

        # plt.plot(distances)
        # plt.show()

        print(len(distances))

        s = max(int(len(distances) * 0.1), 1)

        print("a knee", s, str(datetime.datetime.now()))
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
        print("b good", s, kl.knee, knee, epsilon)

    print("epsilon", epsilon)

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

    # A cluster should be defined if at least it contains a 1% of the samples
    # This implies that at most we define at 100 clusters
    if msamples is None:
        msamples = max(int(len(instr) * 0.1), 1)
    print("msamples", msamples)

    # Compute weights according to the number of instructions and msamples
    # There are invocations that are sufficiently large to be considered a
    # non noise (core sample) regardless of the msamples around them.
    #
    # If a sample contains more than 5% of the total instructions, is
    # considered a cpre sample
    if enable_weighted:
        weights = (instr / (all_instr * 0.05)) * msamples

    print("dbscan", str(datetime.datetime.now()))
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

    # print(instr)
    # print(labels)

    # Compute invocation information
    cluster_count = len(set(labels))
    if -1 in labels:
        cluster_count -= 1  # -1 represents noise points, not clusters
    print("labels", cluster_count)

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
    # Evaluate and tune
    #

    if cluster_count < 10:
        if len(noise_invocations) < 10 and cluster_count == 1 and rec_level < 10:
            # All in the same cluster? Reduce epsilon and msamples
            if fplot == True:
                return dbscan_instr(
                    invocations,
                    epsilon / 10,
                    msamples=msamples / 10,
                    rec_level=rec_level + 1,
                    plotname=plotname,
                    fplot=fplot,
                )
            else:
                return dbscan_instr(
                    instr,
                    epsilon / 10,
                    raw=True,
                    msamples=msamples / 10,
                    rec_level=rec_level + 1,
                    plotname=plotname,
                    fplot=fplot,
                )

        if len(noise_invocations) < (50 - cluster_count):
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
                None,
                raw=True,
                rec_level=rec_level + 1,
                plotname=plotname,
                fplot=False,
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

    instr_coverage, inv_coverage, coverage, centroids = dbscan_instr_eval(
        instr, invocation_sets, noise_invocations, all_instr
    )

    extra_info = {}
    extra_info["instr_coverage"] = instr_coverage
    extra_info["inv_coverage"] = inv_coverage
    extra_info["coverage"] = coverage

    if fplot == True:

        print("instr coverage", instr_coverage)
        print("invocation coverage", inv_coverage)
        print("clusters", cluster_count)
        print("coverage", coverage)

        # plot the final result
        for label, indexes in zip(clusters, invocation_sets):
            for index in indexes:
                labels[index] = label[0]

        for index in noise_invocations:
            labels[index] = -1

        # dbscan_instr_plot(plotname + "instr", instr, instr, labels, centroids)
        # dbscan_instr_plot(plotname + "ipcs", ipcs, ipcs, labels, centroids)
        dbscan_instr_plot(
            plotname + "xy",
            instr,
            ipcs,
            labels,
            centroids,
            instr_coverage,
            inv_coverage,
        )

    assert len(invocation_sets) == len(clusters)
    assert isinstance(invocation_sets, list)
    assert isinstance(clusters, list)
    assert isinstance(noise_invocations, list)
    return ClusteringInformation(
        epsilon, invocation_sets, clusters, noise_invocations, extra=extra_info
    )


def dbscan_instr_plot(
    plotname,
    x,
    y,
    labels,
    centroids,
    instr_coverage,
    inv_coverage,
    custom_range=None,
):

    # colors = ['royalblue', 'maroon', 'forestgreen', 'mediumorchid', 'tan', 'deeppink', 'olive', 'goldenrod', 'lightcyan', 'navy']
    colors = list(matplotlib.colors.CSS4_COLORS.keys())
    colors.remove("red")

    if custom_range is not None:
        to_delete = []
        for idx, (xval, yval, label) in enumerate(zip(x, y, labels)):
            if xval > custom_range[0][1]:
                to_delete.append(idx)
                continue
            if yval > custom_range[1][1]:
                to_delete.append(idx)

        x = np.delete(x, to_delete)
        y = np.delete(y, to_delete)
        labels = np.delete(labels, to_delete)
        centroids = dict(centroids)
        for cent in centroids:
            centroids[cent] = centroids[cent] - len(
                [elem for elem in to_delete if elem < centroids[cent]]
            )

    vectorizer = np.vectorize(lambda x: colors[x % len(colors)])

    # fig, ax = plt.subplots(figsize=(5.5, 5.5))
    fig, ax = plt.subplots()
    title = "Benchmark:" + plotname.split("/")[-4]
    title += " Function:" + plotname.split("/")[-2]

    # the scatter plot:
    ax.scatter(x, y, c=vectorizer(labels), s=1)

    if len(centroids.values()) > 0:
        ax.scatter(
            [x[idx] for idx in centroids.values()],
            [y[idx] for idx in centroids.values()],
            c="red",
            s=10,
            marker="*",
        )

    ax.set_xlabel("Instr")
    ax.set_ylabel("IPC")

    # create new axes on the right and on the top of the current axes
    divider = make_axes_locatable(ax)
    # below height and pad are in inches
    ax_histx = divider.append_axes("top", 1.2, pad=0.1, sharex=ax)
    ax_histy = divider.append_axes("right", 1.2, pad=0.1, sharey=ax)

    text = "Method: densitygrid\n"
    text += "Clusters: %d\n" % len(centroids.keys())
    text += "%% Invocations: %2.2f%%\n" % (inv_coverage * 100)
    text += "%% Instructions: %2.2f%%\n" % (instr_coverage * 100)

    ax.annotate(text, xy=(1.05, 1.2), xycoords="axes fraction")

    # make some labels invisible
    ax_histx.xaxis.set_tick_params(labelbottom=False)
    ax_histy.yaxis.set_tick_params(labelleft=False)

    # now determine nice limits by hand:
    bins = np.arange(0, max(x) * 1.02, max(x) / 100.0)
    ax_histx.hist(x, bins=bins, density=True, stacked=True)
    ax_histx.set_title(title)

    bins = np.arange(0, max(y) * 1.02, max(y) / 100.0)
    ax_histy.hist(y, bins=bins, orientation="horizontal", density=True, stacked=True)

    # the xaxis of ax_histx and yaxis of ax_histy are shared with ax,
    # thus there is no need to manually adjust the xlim and ylim of these
    # axis.

    # ax_histx.set_yticks([0, 0.5, 1])
    # ax_histy.set_xticks([0, 0.5, 1])

    # plt.show()
    plt.savefig(plotname + ".jpg")
    print("plotting", plotname + ".jpg")


def brute_force_2d_density(invocations, epsilon, plotname="plot"):

    max_clusters = 20
    min_clusters_weight = 0.01
    target_coverage = 0.9
    size = 3

    instr = np.array([invocation.metrics.instructions for invocation in invocations])
    ipcs = np.array([invocation.metrics.ipc for invocation in invocations])

    if len(instr) == 0:
        print("no samples")
        exit(0)

    if min(instr) == 0:
        print("sample with 0 instruction")
        exit(0)

    # Remove the outliers (1% top and bottom data points),
    # only if many data # points (>1000) are provided.
    hist_range = [[min(instr), max(instr)], [min(ipcs), max(ipcs)]]
    all_instr = sum(instr)
    if len(instr) > 1000:
        idx = int(len(instr) * 0.001)
        hist_range = [
            [min(sorted(instr)[idx:]) * 0.9, max(sorted(instr)[0:-idx]) * 1.1],
            [min(sorted(ipcs)[idx:]) * 0.9, max(sorted(ipcs)[0:-idx]) * 1.1],
        ]
        all_instr = sum(sorted(instr)[idx:-idx])

    weights = instr / all_instr

    assert min(instr) >= 0
    assert min(ipcs) >= 0

    coverage = 0
    cbins = 101

    while coverage < target_coverage:
        cbins = cbins - 1
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
    # Cluters with at least 1% of the sampled instructions
    #
    combined = [xy for xy in combined if xy[0] >= min_clusters_weight]

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
        # else:
        #    print("nofix")

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
        # else:
        #    print("nofixy")

        # yi = -1
        # xi = -1

        # for xi, (xr1, xr2) in enumerate(xranges):
        # if not xr1 <= instr[sample] < xr2:
        #    continue

        # for yi, (yr1, yr2) in enumerate(yranges):
        #    if yr1 <= ipcs[sample] < yr2:
        #        break

        # print(xi, xidx, cbins)
        # assert(xi == xidx)
        # assert(yi == yidx)

        labels.append(labeldict.get((xidx, yidx), -1))
        if labels[-1] == -1:
            noise_invocations.append(sample)
        else:
            invocation_sets[labels[-1]].append(sample)

    for label in range(len(clusters)):
        assert label in labels, label

    instr_coverage, inv_coverage, coverage, centroids = dbscan_instr_eval(
        instr,
        invocation_sets,
        noise_invocations,
        all_instr,
        ipcs=ipcs,
        custom_range=hist_range[0],
    )

    dbscan_instr_plot(
        plotname + "densexy",
        instr,
        ipcs,
        labels,
        centroids,
        instr_coverage,
        inv_coverage,
        custom_range=hist_range,
    )

    extra_info = {}
    extra_info["instr_coverage"] = instr_coverage
    extra_info["inv_coverage"] = inv_coverage
    extra_info["coverage"] = coverage

    print("clusters", len(clusters))

    return ClusteringInformation(
        epsilon, invocation_sets, clusters, noise_invocations, extra=extra_info
    )


def dbscan_instr_eval(
    instr, invocation_sets, noise_invocations, all_instr, ipcs=None, custom_range=None
):
    # Evaluate the percentage of instructions in a cluster as well as the
    # percentage of non noise invocations. These two metrics can be used to
    # assess the quality of the clustering. Also, select the cluster
    # representatives as the "centroid", according to geometric mean depending
    # on the dimensions given.
    coverage = {}
    centroids = {}

    if custom_range is None:
        coverage[-1] = sum(instr[i] for i in noise_invocations) / all_instr
    else:
        coverage[-1] = (
            sum(
                [
                    instr[i]
                    for i in noise_invocations
                    if instr[i] >= custom_range[0] and instr[i] < custom_range[1]
                ]
            )
            / all_instr
        )

    for label in range(len(invocation_sets)):
        coverage[label] = sum([instr[i] for i in invocation_sets[label]]) / all_instr

        assert len(invocation_sets[label]) > 0
        gmean = np.exp(np.log([instr[i] for i in invocation_sets[label]]).mean())
        if ipcs is not None:
            gmean2 = np.exp(np.log([ipcs[i] for i in invocation_sets[label]]).mean())

        centroid = None
        centroid_diff = None

        for i in invocation_sets[label]:

            diff = abs(instr[i] - gmean)
            if ipcs is not None:
                diff2 = abs(ipcs[i] - gmean2)
                diff = math.sqrt(diff ** 2 + diff2 ** 2)

            if centroid is None:
                centroid = i
                centroid_diff = diff
                continue
            if diff < centroid_diff:
                centroid = i
                centroid_diff = diff
        centroids[label] = centroid

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
                    if instr[i] >= custom_range[0] and instr[i] < custom_range[1]
                ]
            )
            / max(
                min(
                    len(
                        [
                            x
                            for x in instr
                            if instr[i] >= custom_range[0]
                            and instr[i] < custom_range[1]
                        ]
                    ),
                    len(instr),
                ),
                1,
            )
        )

    return instr_coverage, inv_coverage, coverage, centroids
