/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 IBM Corporation
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

#include <stdlib.h>

void add(unsigned int N, int *a, int *b, int *c) {
    for (unsigned int i = 0; i < N; i++)
       c[i] = a[i] + b[i];
}

int main() {
    const unsigned int max_n = 100000000;
    int *a = malloc(sizeof(int) * max_n);
    int *b = malloc(sizeof(int) * max_n);
    int *c = malloc(sizeof(int) * max_n);

    for (unsigned int i = 0; i < 10; i++)
        add(i % 2 ? max_n : max_n / 100, a, b, c);

    return 0;
}
