include (CheckLibraryExists)
include (CheckIncludeFiles)

CHECK_INCLUDE_FILES("${CHOPSTIX_PERFMON_PREFIX}/include/perfmon/pfmlib.h" HAVE_PFM_HEADER)
if (NOT HAVE_PFM_HEADER)
    message (FATAL_ERROR "perfmon2 headers not found in '${CHOPSTIX_PERFMON_PREFIX}'")
endif ()

CHECK_LIBRARY_EXISTS(pfm pfm_initialize  ${CHOPSTIX_PERFMON_PREFIX}/lib/ HAVE_PFM_LIB)
if (NOT HAVE_PFM_LIB)
    message (FATAL_ERROR "perfmon2 lib in '${CHOPSTIX_PERFMON_PREFIX}' does not implement the required 'pfm_initialize'")
endif ()

# CHECK_LIBRARY_EXISTS(pfm pfm_terminate ${CHOPSTIX_PERFMON_PREFIX}/lib/ HAVE_PFM_LIB)
# if (NOT HAVE_PFM_LIB)
#     message (FATAL_ERROR "perfmon2 lib in '${CHOPSTIX_PERFMON_PREFIX}' does not implement the required 'pfm_terminate'")
# endif ()

# CHECK_LIBRARY_EXISTS(pfm pfm_strerror ${CHOPSTIX_PERFMON_PREFIX}/lib/ HAVE_PFM_LIB)
# if (NOT HAVE_PFM_LIB)
#     message (FATAL_ERROR "perfmon2 lib in '${CHOPSTIX_PERFMON_PREFIX}' does not implement the required 'pfm_strerror'")
# endif ()

# CHECK_LIBRARY_EXISTS(pfm pfm_get_os_event_encoding ${CHOPSTIX_PERFMON_PREFIX}/lib/ HAVE_PFM_LIB)
# if (NOT HAVE_PFM_LIB)
#    message (FATAL_ERROR "perfmon2 lib in '${CHOPSTIX_PERFMON_PREFIX}' does not implement the required 'pfm_get_os_event_encoding'")
# endif ()

# CHECK_LIBRARY_EXISTS(pfm perf_event_open ${CHOPSTIX_PERFMON_PREFIX}/lib/ HAVE_PFM_LIB)
# if (NOT HAVE_PFM_LIB)
#    message (FATAL_ERROR "perfmon2 lib in '${CHOPSTIX_PERFMON_PREFIX}' does not implement the required 'perf_event_open'")
# endif ()
