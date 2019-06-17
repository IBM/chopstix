# ChopStiX

Extract representative microbenchmarks.

[![Build Status](https://travis-ci.org/IBM/chopstix.svg?branch=master)](https://travis-ci.org/IBM/chopstix)

## Installation

The following is a list of the minimal software requirements to be able to
install ChopStiX.

- A compiler with C++11 support (e.g. gcc 4.8.4)
- libpfm-4.8.0: http://perfmon2.sourceforge.net/
- CMake >= 2.8
- Git
- Python 2.7 (optional)
- sqlite3 (optional)

To download and setup ChopStiX for installation follow these steps:

    git clone .../chopstix.git chopstix
    cd chopstix
    git submodule sync 
    git submodule update --init --recursive

### Support script

We provide a support script that facilitates the installation process. If you
have the right compiler and [Cmake], this script will install ChopStiX and its
dependencies in the specified installation directory:

    ./install.sh <prefix>

### Manual compilation

ChopStiX uses [CMake] as build system. We have provided a simple wrapper
in form of a configure script in order to provide a more accessible interface.

The basic build workflow is as follows:

    mkdir build
    cd build
    ../configure
    make && make install

For more detailed information regarding configuration options see docs/installation.md.

## Basic usage

ChopStiX saves all collected information in a local SQL database.
By default it will save data to `chop.db`. Most commands have a `-data`
option to change this path.

In general, you can invoke any command using `chop <command>`.
For more information about a specific command, try `chop help <command>`.
There are also some utility scripts (i.e. `cx-trace.sh`) which are generally
prefixed by `cx-`.

The basic workflow for ChopStiX is as follows:
    
    chop sample ./my_app     # Sample invocation of ./my_app
    chop disasm              # Detect and disassemble used object files
    chop count               # Group and count samples per instruction
    chop annotate            # Annotate control flow graph
    chop search -target-coverage 90%   # Generate hottest paths
    chop list paths          # List generated paths
    chop text path -id <id>  # Show instructions for path with <id>


For a more detailed workflow example see the following documents:

- [Usage](docs/usage.md)
- [Tracing](docs/tracing.md)
- [Installation](docs/installation.md)
- [Custom_commands](docs/custom_commands.md)
- [Database layout](docs/database_layout.md)
- [Debug](docs/debug.md)
- [Events power8](docs/events_power8.md)

[CMake]: https://cmake.org/
