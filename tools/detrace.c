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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

int main(int argc, const char **argv) {

    if(argc != 2){
        fprintf(stderr, "Usage: chop-detrace <bin_file>\n");
        if (argc == 1) { exit(0); }
        exit(-1);
    }

    fprintf(stderr, "chop-detrace: Opening '%s' ...\n", argv[1]);
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "chop-detrace: Unable to read '%s'\n", argv[1]);
        fprintf(stderr, "chop-detrace: Error: %s\n",strerror(errno));
        exit(-1);
    }

    long dat;
    int trace_id = 0;
    while (fread(&dat, sizeof(long), 1, fp) > 0) {
        switch (dat) {
        case -1: printf("# trace %d\n", trace_id); break;
        case -2: ++trace_id; break;
        default: printf("%lx\n", dat);
        };
    }
}
