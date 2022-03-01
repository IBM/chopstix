include (CheckLibraryExists)
include (CheckIncludeFiles)

CHECK_INCLUDE_FILES("${CHOPSTIX_ZLIB_PREFIX}/include/zlib.h" HAVE_ZLIB_HEADER)
if (NOT HAVE_ZLIB_HEADER)
    message (FATAL_ERROR "zlib headers not found in '${CHOPSTIX_ZLIB_PREFIX}'")
endif ()

CHECK_LIBRARY_EXISTS(z zlibVersion ${CHOPSTIX_ZLIB_PREFIX}/lib/ HAVE_ZLIB_LIB)
if (NOT HAVE_ZLIB_LIB)
    message (FATAL_ERROR "zlib in '${CHOPSTIX_ZLIB_PREFIX}' does not implement the required 'zlibVersion'")
endif ()
