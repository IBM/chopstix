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
    if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "ppc64")
        SET(CHOPSTIX_POWER_SUPPORT ON)
        message (STATUS "Current architecture is ppc64")
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "ppc64le")
        SET(CHOPSTIX_POWERLE_SUPPORT ON)
        message (STATUS "Current architecture is ppc64le")
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "s390x")
        SET(CHOPSTIX_SYSZ_SUPPORT ON)
        message (STATUS "Current architecture is s390x")
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        SET(CHOPSTIX_X86_SUPPORT ON)
        message (STATUS "Current architecture is x86_64")
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "riscv64")
        SET(CHOPSTIX_RISCV_SUPPORT ON)
        message (STATUS "Current architecture is riscv64")
    else()
        message (FATAL_ERROR "Support for one architecture required. Define: CHOPSTIX_POWER_SUPPORT, CHOPSTIX_POWERLE_SUPPORT, CHOPSTIX_X86_SUPPORT or CHOPSTIX_SYSZ_SUPPORT")
    endif()
else()
    message (STATUS "Target architecture forced by parameter")
    SET(ARCH_SUPPORT 0)
    if (${CHOPSTIX_RISCV_SUPPORT} STREQUAL "ON")
        MATH(EXPR ARCH_SUPPORT "${ARCH_SUPPORT}+1")
        set(CMAKE_SYSTEM_PROCESSOR "riscv64")
    endif()

    if (${CHOPSTIX_X86_SUPPORT} STREQUAL "ON")
        MATH(EXPR ARCH_SUPPORT "${ARCH_SUPPORT}+1")
        set(CMAKE_SYSTEM_PROCESSOR "x86_64")
    endif()

    if (${CHOPSTIX_SYSZ_SUPPORT} STREQUAL "ON")
        MATH(EXPR ARCH_SUPPORT "${ARCH_SUPPORT}+1")
        set(CMAKE_SYSTEM_PROCESSOR "s390x")
    endif()

    if (${CHOPSTIX_POWER_SUPPORT} STREQUAL "ON")
        MATH(EXPR ARCH_SUPPORT "${ARCH_SUPPORT}+1")
        set(CMAKE_SYSTEM_PROCESSOR "ppc64")
    endif()

    if (${CHOPSTIX_POWERLE_SUPPORT} STREQUAL "ON")
        MATH(EXPR ARCH_SUPPORT "${ARCH_SUPPORT}+1")
        set(CMAKE_SYSTEM_PROCESSOR "ppc64le")
    endif()

    if (${ARCH_SUPPORT} STREQUAL 0)
        message (FATAL_ERROR "Support for one architecture required. Define: CHOPSTIX_POWER_SUPPORT, CHOPSTIX_POWERLE_SUPPORT, CHOPSTIX_X86_SUPPORT or CHOPSTIX_SYSZ_SUPPORT")
    endif()

    if (NOT(${ARCH_SUPPORT} STREQUAL 1))
        message (FATAL_ERROR "Support for multiple architectures not supported")
    endif()
endif ()

echo_support(POWER "IBM POWER")
echo_support(POWERLE "IBM POWER (little endian)")
echo_support(X86 "x86_64")
echo_support(SYSZ "IBM System Z")
echo_support(RISCV "RiscV")
