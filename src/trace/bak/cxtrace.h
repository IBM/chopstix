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
/*****************************************************************
 * NAME        : cxtrace.h
 * DESCRIPTION : Public interface to enable tracing with chopstix
 *****************************************************************/

#ifndef CXTRACE_H
#define CXTRACE_H

typedef enum cxtrace_status {
    CXTRACE_OK = 0,
    CXTRACE_EINVAL,  // Invalid argument
    CXTRACE_EPROT,   // Protocol error
    CXTRACE_ESPACE,  // Insufficient space
    CXTRACE_EOPEN,   // Unable to open file
    CXTRACE_EIMPL,   // Not implemented
} cxtrace_status;

typedef struct cxtrace_config {
    char *log_path;        // Path to log file
    char *with_perm;       // Filter memory regions by permission
    char *trace_path;      // Path to trace file
    int save_content : 1;  // Set to 1 if saving page contents
} cxtrace_config;

cxtrace_status cxtrace_init(void);
void cxtrace_quit(void);

cxtrace_status cxtrace_get_config(cxtrace_config *cfg);
cxtrace_status cxtrace_set_config(cxtrace_config *cfg);

cxtrace_status cxtrace_start_trace(void);
cxtrace_status cxtrace_stop_trace(void);

const char *cxtrace_strerror(cxtrace_status err);
const char *cxtrace_version(void);

#endif /* CXTRACE_H */
