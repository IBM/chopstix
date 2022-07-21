# Installation guide

ChopStiX comes with a configuration script (`configure`).
This sets common build options and calls CMake.
For a list of all available options see `./configure --help`.

To learn how to use and configure CMake to build ChopStiX
read the remainder of this document.

## CMake quickstart

ChopStiX uses [CMake] as build system. The basic build workflow is as follows.
Change `<prefix>` for the location where you want to install ChopStiX.

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=<prefix> ..
    make
    make install

If you need to change the default compiler and/or compilation flags, 
you can preset custom settings as following:

    CXX=<your_compiler> CXXFLAGS=<cxxflags> CFLAGS=<cflags> LDFLAGS=<ldflags> cmake -DCMAKE_INSTALL_PREFIX=<prefix> ..

Use `make VERBOSE=1`, if you need extra output from the compilation
process. More details on other options, such as the ones that control the 
target architecture to support (RISCV, PowerPC, PowerPC LE, Z or x86) follow.

## Build options

By default ChopStiX will try to detect the current architecture.
You can specify which architecture support you want to enable with
the following options:

- `CHOPSTIX_POWER_SUPPORT=[ON|OFF]`
- `CHOPSTIX_POWERLE_SUPPORT=[ON|OFF]`
- `CHOPSTIX_SYSZ_SUPPORT=[ON|OFF]`
- `CHOPSTIX_X86_SUPPORT=[ON|OFF]`
- `CHOPSTIX_RISCV_SUPPORT=[ON|OFF]`

ChopStiX has three main external dependencies: sqlite3, perfmon2 and zlib.
The following options allow you to configure the build for your setup.

- `CHOPSTIX_PERFMON_PREFIX=[PATH]`: Path of perfmon2 installation
- `CHOPSTIX_SQLITE_PREFIX=[PATH]`: Path of sqlite installation
- `CHOPSTIX_ZLIB_PREFIX=[PATH]`: Path of zlib installation

Additionally, sqlite3 comes bundled with ChopStiX, so you can also
manually build it.

- `CHOPSTIX_BUILD_SQLITE=[ON|OFF]`

If you are planning on modifying the source code, it is recommended
that you generate test targets.

- `CHOPSTIX_BUILD_TESTS=[ON|OFF]`

## External tools

In order to have a complete ChopStiX functionality the following extra
tools/dependencies are fetched and installed during the installation
process:

- [Microprobe](https://github.com/IBM/microprobe) to be able to convert
  the generated Microprobe Test files (MPTs) to other format after tracing.
- [Valgrind](https://valgrind.org/) to be able to complete the generated
  MPTs with detailed memory access trace information.

### Useful CMake options

These are common CMake options that might be useful.

- `CMAKE_INSTALL_PREFIX=[PATH]`: Directory where you want to install ChopStiX.
- `CMAKE_BUILD_TYPE` : Sets various default flags (e.g. `NDEBUG`).
    - Release, RelWithDebInfo, Debug, Profile
- `CMAKE_CXX_COMPILER`, `CMAKE_CXX_FLAGS`
