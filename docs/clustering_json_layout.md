# Layout of the JSON file describing the clustering information

The `cti_cluster` tool generates a JSON describing the clusters found of the
invocations to a particular function of a benchmark. This document describes
its layout.

The root of the document is an object, having the following mandatory fields:

- `epsilon`: The epsilon parameter used when clustering. It is a number.
- `clusters`: The clusters found when clustering. It is an array in which every
              element is a cluster. Each element is, in turn, an array in which
              every element is the index of an invocation set found in the
              `invocation_sets` field described later. Clusters are 0-indexed.
- `noise_invocations`: The invocation sets which are considered noise points
                       when clustering. It is an array in which each element is
                       the index of an invocation set found in the
                       `invocation_sets` field described later. Noise
                       invocations are 0-indexed.
- `invocation_sets`: The invocation sets found within all the invocations to a
                     function. It is an array in which each element is an
                     invocation set. Invocation sets are 0-indexed. Each
                     element is, in turn, an array in which each element is the
                     index of an invocation to the function. Invocations are
                     0-indexed.


The root of the document also has the following optional fields:

- `extra`: TODO document this field

Example:

```json
{
    "epsilon": 0.5,
    "clusters": [[1, 3], [0]],
    "noise_invocations": [2, 4],
    "invocation_sets": [
        [0, 1, 2],
        [3, 4, 5],
        [6, 7, 8],
        [9, 10, 11],
        [12, 13, 14]
    ]
}
```

In this example there are two clusters, 0 and 1.

- Cluster 0 has 6 invocations, corresponding to those found in invocation sets
  1 and 3: 3, 4, 5, 9, 10, 11.
- Cluster 1 has 3 invocations, corresponding to invocation set 0: 0, 1, 2.

In this example there are 6 noise invocations, found in invocation sets 2 and 4.

