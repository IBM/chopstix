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
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define TYPE double

#define die(ARG)                      \
    do {                              \
        fputs(ARG, stderr);           \
        fputs("\n", stderr);          \
        exit(-1);                     \
    } while (0)

#define streq(a, b) (strcmp((a), (b)) == 0)

double rand_next() { return (rand() + 0.0) / RAND_MAX; }
double get_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + 1e-6 * tv.tv_usec;
}

TYPE daxpy(TYPE *x, TYPE *y, int n, TYPE a);

void init_test(int num_elems, double **x, double **y, double *a) {
    srand(19940617);
    *a = rand_next();
    long pagesize = sysconf(_SC_PAGESIZE);
    *x = memalign(pagesize, num_elems * sizeof(TYPE));
    *y = memalign(pagesize, num_elems * sizeof(TYPE));

    for (int i = 0; i < num_elems; ++i) {
        (*x)[i] = (*y)[i] = (TYPE)rand_next();
    }
}

void quit_test(double *x, double *y) {
    printf("Result: %g\n", y[0]);
    free(y);
    free(x);
}

void print_info(int num_elems, double *x, double *y, int num_iter) {
    long pagesize = sysconf(_SC_PAGESIZE);
    printf("# vec_x %lx\n", (long)x);
    printf("# vec_y %lx\n", (long)y);
    printf("# num_pages %d\n",
           (int)ceil(num_elems * sizeof(TYPE) * 1.0 / pagesize));
    printf("# num_iter %d\n", num_iter);
}

void test_time(int num_elems, double *x, double *y, double a, double time_sec) {
    double time_start = get_seconds();
    do {
        daxpy(x, y, num_elems, a);
    } while (get_seconds() - time_start < time_sec);
}

void test_iter(int num_elems, double *x, double *y, double a, int num_iter) {
    print_info(num_elems, x, y, num_iter);
    for (int i = 0; i < num_iter; ++i) {
        daxpy(x, y, num_elems, a);
    }
}

int main(int argc, char **argv) {
    const char *usage =
        "Usage: test-daxpy <size_kb> iter <num_iter>\n"
        "       test-daxpy <size_kb> inst <num_iter>\n"
        "       test-daxpy <size_kb> time <time_sec>";

    if (argc != 4) die(usage);

    double size_kb = atof(argv[1]);
    int num_elems = size_kb * 1024 / sizeof(TYPE);

    TYPE *x, *y, a;
    init_test(num_elems, &x, &y, &a);

    if (streq(argv[2], "iter")) {
        test_iter(num_elems, x, y, a, atof(argv[3]));
    } else if (streq(argv[2], "time")) {
        test_time(num_elems, x, y, a, atoi(argv[3]));
    } else {
        die(usage);
    }

    quit_test(x, y);
}

TYPE daxpy(TYPE *x, TYPE *y, int n, TYPE a) {
    for (int i = 0; i < n; ++i) {
        y[i] = a * x[i] + y[i];
    }
    return y[0];
}
