# Use default option or environment variable
macro (set_env name def)
    if ("$ENV{${name}}none" STREQUAL "none")
        set (${name} ${def} ${ARGN})
    else()
        set (${name} $ENV{${name}} ${ARGN})
    endif()
endmacro()
