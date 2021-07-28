import os
from sklearn.cluster import DBSCAN
from dbscan1d.core import DBSCAN1D
from sklearn import metrics
import numpy as np
import math
import random
import json

from src.distance import disjoint_sets

class ClusteringInformation:

    def __init__(self, epsilon, invocation_sets, clusters, noise_invocations):
        self._epsilon = epsilon
        self._invocation_sets = invocation_sets
        self._clusters = clusters
        self._noise_invocations = noise_invocations

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
            "noise_invocations": self._noise_invocations
        }

        with open(path, "w") as file:
            json.dump(json_obj, file)

    @staticmethod
    def from_file(path):
        with open(path, "r") as file:
            json_obj = json.load(file)

        return ClusteringInformation(json_obj['epsilon'], json_obj['invocation_sets'], json_obj['clusters'], json_obj['noise_invocations'])


def estimate_dbscan_epsilon(trace, coverage):
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

    return (distanceLimit + nextDist)/2

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

    print('Estimated number of clusters: %d' % n_clusters_)
    print('Estimated number of noise points: %d (%f%%)' % (n_noise_, n_noise_*100/trace.get_invocation_set_count()))
    if n_clusters_ > 1:
        silhouette_score = metrics.silhouette_score(distance_matrix, labels,
                                                    metric="precomputed")
        print("Silhouette Coefficient: %0.3f" % silhouette_score)

    clusters = [[] for i in range(n_clusters_)]
    noise_invocations = []
    for i in range(len(labels)):
        label = labels[i]

        if label == -1:
            noise_invocations.append(i)
        else:
            clusters[label].append(i)

    invocation_sets = [invocation_set.invocations for invocation_set in trace.invocation_sets]

    return ClusteringInformation(epsilon, invocation_sets, clusters, noise_invocations)

def dbscan_ipc(invocations, epsilon):
    # init dbscan object
    dbs = DBSCAN1D(eps=epsilon)

    ipcs = np.array([invocation.metrics.ipc for invocation in invocations])
    print(ipcs)

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
            invocation_sets.append([i])
            noise_invocations.append(len(invocation_sets) - 1)
        else:
            invocation_sets[label].append(i)

    return ClusteringInformation(epsilon, invocation_sets, clusters, noise_invocations)
