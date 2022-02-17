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

import os
from setuptools import setup, find_packages


def read(fname):
    """Read a file"""
    return open(fname).read()


setup(
    name="chopstix",
    version="0.0rc0",
    # packages = find_packages(include=["*.py","src"]),
    description="ChopStiX Clustering and Support python tools",
    # package_dir={'': os.path.join(".",'src')},
    packages=["src"],
    py_modules=[
        "cti_accesses",
        "cti_cluster_info",
        "cti_cluster",
        "cti_histogram",
        "cti_micro_weight",
        "cti_perf_summary",
        "cti_trace_info",
        "lineplot",
    ],
    entry_points={
        "console_scripts": [
            "cti_accesses = cti_accesses:main",
            "cti_cluster_info = cti_cluster_info:main",
            "cti_cluster = cti_cluster:main",
            "cti_histogram = cti_histogram:main",
            "cti_micro_weight = cti_micro_weight:main",
            "cti_perf_summary = cti_perf_summary:main",
            "cti_trace_info = cti_trace_info:main",
            "lineplot = lineplot:main",
        ]
    },
    install_requires=[
        "scikit-learn>=0.24.2",
        "matplotlib>=3.3.4",
        "numpy>=1.19.5",
        "dbscan1d>=0.1.6",
        "kneed>=0.7.0",
        "black>=22.1.0",
    ],
    package_data={"": ["../README.md"]},
    long_description=read(os.path.join(".", "README.md")),
    zip_safe=False,
    platforms=["none-any"],
    url="https://github.com/IBM/chopstix/",
)
