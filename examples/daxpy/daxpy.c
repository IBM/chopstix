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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LOG(...)                      \
    do {                              \
        fprintf(stdout, "-- ");  \
        fprintf(stdout, __VA_ARGS__); \
        fprintf(stdout, "\n");        \
    } while (0)

double daxpy(double *x, double *y, int n, double a) {
    for (int i = 0; i < n; ++i) {
        y[i] = a * x[i] + y[i];
    }
    return y[0];
}

double rand_next() { return (rand() + 0.0) / RAND_MAX; }

int main(int argc, char **argv) {
    int n = getenv("TEST_SIZE") ? atoi(getenv("TEST_SIZE")) : 20000;
    int iter = getenv("TEST_ITER") ? atoi(getenv("TEST_ITER")) : 100000;
    double a = rand_next();

    LOG("allocating memory (size: %d)", n);
    double *x = malloc(sizeof(double) * n);
    double *y = malloc(sizeof(double) * n);

    LOG("x: %p   y: %p", x, y);
    srand(19940617);
    for (int i = 0; i < n; ++i) {
        x[i] = rand_next();
        y[i] = rand_next();
    }

    LOG("starting daxpy (iter: %d)", iter);

    for (int i = 0; i < iter; ++i) {
        daxpy(x, y, n, a);
    }

    LOG("done (y[0] = %.1f)", y[0]);
    free(y);
    free(x);
}
