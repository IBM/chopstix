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

#ifdef DEBUG_MODE
#define DEBUG 1
#else
#define DEBUG 0
#endif

// TODO: This is experimental, multi process tracing has not been fully
// tested. Enable at your own risk ;)
#define MULTIPROCESS 0

#define debug_print(fmt, ...) \
	do { if (DEBUG) fprintf(stderr, "DEBUG:%s:%d:%s(): " fmt, __FILE__, \
    __LINE__, __func__,##__VA_ARGS__); } while (0);

#define MAX_SAMPLES (1024*1024)
#define MAX_BREAKPOINTS 1024

#define FORK_OR_CLONE(status) \
    ((status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))) || \
    (status>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8))) || \
    (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))))


// TODO: Ideally, after detecting the child, we should waitfor it (it
// starts, restore their memory contents (breakpoints, in case they are
// enabled --ie. contents validated--) and then detach.
#define check_child(ret, pid, status) \
    do { \
        unsigned long cpid = 0; \
        if (FORK_OR_CLONE(status)) { \
            ret = ptrace(PTRACE_GETEVENTMSG, pid, NULL, &cpid); \
            if (ret == -1) { perror("ERROR: while exectuing PTRACE_GETEVENTMSG"); kill(pid, SIGKILL); exit(EXIT_FAILURE);}; \
            fprintf(stderr, "WARNING: Process fork/clone detected with PID: %ld\n", cpid); \
            ret = ptrace(PTRACE_DETACH, cpid, NULL, NULL); \
            if (ret == -1) { perror("ERROR: while exectuing PTRACE_DETACH"); kill(cpid, SIGKILL); kill(pid, SIGKILL); exit(EXIT_FAILURE);}; \
        } \
    } while (0);

