/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2019 IBM Corporation
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
*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define log(p, ...)                       \
    do {                                  \
            fprintf(stderr, "=%s= ", p);  \
            fprintf(stderr, __VA_ARGS__); \
            fprintf(stderr, "\n");        \
            fflush(stderr);               \
    } while (0)

#define logt(...) log("t", __VA_ARGS__)

double daxpy(double *x, double *y, int n, double a) {
    for (int i = 1; i < n; ++i) {
        y[i] = a * x[i] + y[i-1];
    }
    return y[n-1];
}

double rand_next() { return (rand() + 0.0) / RAND_MAX; }

int main(int argc, char **argv) {
    int iter = getenv("TEST_ITER") ? atoi(getenv("TEST_ITER")) : 10;
    int n = getenv("TEST_SIZE") ? atoi(getenv("TEST_SIZE")) : 10000;
    double a = rand_next();
    logt("allocating memory (size: %d)", n);
    double *x = malloc(sizeof(double) * n);
    double *y = malloc(sizeof(double) * n);
    logt("x: %p   y: %p", x, y);
    srand(19940617);
    for (int i = 0; i < n; ++i) {
        x[i] = rand_next();
        y[i] = rand_next();
    }
    logt("starting daxpy (iter: %d)", iter);
    for (int i = 0; i < iter; ++i) {
        daxpy(x, y, n, a);
    }
    logt("done (y[0] = %.1f)", y[0]);
    free(y);
    free(x);
}
