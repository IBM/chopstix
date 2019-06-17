macro(add_chopstix_library name sources)
    set(_sources ${sources} ${ARGN})
    add_library(cx-${name} STATIC
        ${_sources})
    set_target_properties(cx-${name}
        PROPERTIES COMPILE_FLAGS "-fPIC")
endmacro()
