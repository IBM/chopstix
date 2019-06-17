macro(add_clang_format target sources)
    set(_sources ${sources} ${ARGN})
    add_custom_target(${target}
        COMMAND clang-format
        -i
        ${_sources}
    )
endmacro()
