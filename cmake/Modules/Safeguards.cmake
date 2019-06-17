# build defaults
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "RelWithDebInfo")
endif ()

# no in-source builds
if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message (FATAL_ERROR "In-source builds are not allowed.")
endif ()

# check build type
string (TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)
if (NOT cmake_build_type_lower STREQUAL "debug" AND
    NOT cmake_build_type_lower STREQUAL "release" AND
    NOT cmake_build_type_lower STREQUAL "relwithdebinfo")
    message (FATAL_ERROR "Unsupported build type '${CMAKE_BUILD_TYPE}'.")
endif ()
