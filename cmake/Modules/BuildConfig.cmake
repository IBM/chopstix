# ------------- #
# Build options #
# ------------- #

include (SetEnv) # Use either default or env variable

set_env (CHOPSTIX_BUILD_TESTS OFF CACHE BOOL "Generate build targets for testing." )
set_env (CHOPSTIX_BUILD_EXAMPLES OFF CACHE BOOL "Generate build targets for examples." )

# TODO Support for automatic documentation (e.g. doxygen)
set_env (CHOPSTIX_BUILD_DOCS OFF CACHE BOOL "Generate build targets for documentation." )

set_env (CHOPSTIX_POWER_SUPPORT OFF CACHE BOOL "Architecture support for PowerPC." )
set_env (CHOPSTIX_POWERLE_SUPPORT OFF CACHE BOOL "Architecture support for PowerPC LE." )
set_env (CHOPSTIX_SYSZ_SUPPORT OFF CACHE BOOL "Architecture support for zArch ." )
set_env (CHOPSTIX_X86_SUPPORT OFF CACHE BOOL "Architecture support for x86." )

set_env (CHOPSTIX_BUILD_SQLITE OFF CACHE BOOL "Generate build targets for sqlite3." )
set_env (CHOPSTIX_SQLITE_PREFIX "" CACHE PATH "Path to sqlite3 installation.")
set_env (CHOPSTIX_PERFMON_PREFIX "" CACHE PATH "Path to Perfmon2 installation.")

set_env (CHOPSTIX_PROFILE_CPU OFF CACHE BOOL "Enable profiling of CPU performance.")
