# - Try to find Perfmon2
# Once done this will define
#
#  PERFMON2_FOUND - system has Perfmon2
#  PERFMON2_INCLUDE_DIRS - the Perfmon2 include directory
#  PERFMON2_LIBRARIES - Link these to use Perfmon2
#  PERFMON2_DEFINITIONS - Compiler switches required for using Perfmon2
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (PERFMON2_LIBRARIES AND PERFMON2_INCLUDE_DIRS)
  # in cache already
  set(PERFMON2_FOUND TRUE)
else (PERFMON2_LIBRARIES AND PERFMON2_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  find_path(PERFMON2_INCLUDE_DIR
    NAMES
      perfmon/pfmlib.h
      perfmon/perf_event.h
      perfmon/pfmlib_perf_event.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(PERFMON2_LIBRARY
    NAMES
      pfm
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (PERFMON2_LIBRARY)
    set(PERFMON2_FOUND TRUE)
  endif (PERFMON2_LIBRARY)

  set(PERFMON2_INCLUDE_DIRS
    ${PERFMON2_INCLUDE_DIR}
  )

  if (PERFMON2_FOUND)
    set(PERFMON2_LIBRARIES
      ${PERFMON2_LIBRARIES}
      ${PERFMON2_LIBRARY}
    )
  endif (PERFMON2_FOUND)

  if (PERFMON2_INCLUDE_DIRS AND PERFMON2_LIBRARIES)
     set(PERFMON2_FOUND TRUE)
  endif (PERFMON2_INCLUDE_DIRS AND PERFMON2_LIBRARIES)

  if (PERFMON2_FOUND)
    if (NOT Perfmon2_FIND_QUIETLY)
      message(STATUS "Found Perfmon2: ${PERFMON2_LIBRARIES}")
    endif (NOT Perfmon2_FIND_QUIETLY)
  else (PERFMON2_FOUND)
    if (Perfmon2_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Perfmon2")
    endif (Perfmon2_FIND_REQUIRED)
  endif (PERFMON2_FOUND)

  # show the PERFMON2_INCLUDE_DIRS and PERFMON2_LIBRARIES variables only in the advanced view
  mark_as_advanced(PERFMON2_INCLUDE_DIRS PERFMON2_LIBRARIES)

endif (PERFMON2_LIBRARIES AND PERFMON2_INCLUDE_DIRS)

