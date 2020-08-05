# Check architecture support
# At least one of these must be enabled as supported:
# - POWER -or- POWER LE
# - System Z
# - X86
# CMake tries to detect the architecture if none are defined

macro (echo_support name docstring)
    if (CHOPSTIX_${name}_SUPPORT)
        message (STATUS "Building with support for ${docstring}")
    endif()
endmacro()

if (NOT(CHOPSTIX_POWER_SUPPORT OR CHOPSTIX_POWERLE_SUPPORT
    OR CHOPSTIX_X86_SUPPORT OR CHOPSTIX_SYSZ_SUPPORT OR CHOPSTIX_RISCV_SUPPORT))
    message (STATUS "Detecting target architecture ...")
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ppc64")
        SET(CHOPSTIX_POWER_SUPPORT ON)
        message (STATUS "Current architecture is ppc64")
    elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ppc64le")
        SET(CHOPSTIX_POWERLE_SUPPORT ON)
        message (STATUS "Current architecture is ppc64le")
    elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "s390x")
        SET(CHOPSTIX_SYSZ_SUPPORT ON)
        message (STATUS "Current architecture is s390x")
    elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        SET(CHOPSTIX_X86_SUPPORT ON)
        message (STATUS "Current architecture is x86_64")
    elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "riscv64")
        SET(CHOPSTIX_RISCV_SUPPORT ON)
        message (STATUS "Current architecture is riscv64")
    else()
        message (FATAL_ERROR "Support for one architecture required. Define: CHOPSTIX_POWER_SUPPORT, CHOPSTIX_POWERLE_SUPPORT, CHOPSTIX_X86_SUPPORT or CHOPSTIX_SYSZ_SUPPORT")
    endif()
endif ()

echo_support(POWER "IBM POWER")
echo_support(POWERLE "IBM POWER (little endian)")
echo_support(X86 "x86_64")
echo_support(SYSZ "IBM System Z")
echo_support(RISCV "RiscV")
