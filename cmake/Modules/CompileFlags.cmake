
set (C_STANDARD 99)
set (C_STANDARD_REQUIRED ON)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused -std=gnu99 -Werror")
set (CMAKE_C_FLAGS_DEBUG "-g -O0 -fno-omit-frame-pointer")
set (CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

set (CXX_STANDARD 11)
set (CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused -Wno-sequence-point -std=gnu++11 -Werror")
set (CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-omit-frame-pointer")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

add_definitions ("-D_GNU_SOURCE")

