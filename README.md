# ChopStiX

Extract representative microbenchmarks.

[![Build Status](https://travis-ci.org/IBM/chopstix.svg?branch=master)](https://travis-ci.org/IBM/chopstix)

![GitHub](https://img.shields.io/github/license/IBM/chopstix.svg)
![GitHub forks](https://img.shields.io/github/forks/IBM/chopstix.svg?style=social)
![GitHub stars](https://img.shields.io/github/stars/IBM/chopstix.svg?style=social)
![GitHub watchers](https://img.shields.io/github/watchers/IBM/chopstix.svg?style=social)

## Quick start

Execute the following command to install ChopStiX:

    ./install.sh <INSTALLATION_DIRECTORY>

The command will perform all the necessary steps (i.e. including the download
of specific requirements) and install ChopStix in `<INSTALLATION_DIRECTORY>.

## Installation

The following is a list of the minimal software requirements to be able to
install ChopStiX.

- A compiler with C++11 support (e.g. gcc 4.8.4)
- libpfm-4.8.0: http://perfmon2.sourceforge.net/
- CMake >= 2.8
- Git
- Python 3 (optional)
- sqlite3 (optional)

To download and setup ChopStiX for installation follow these steps:

    git clone https://github.com/IBM/chopstix.git chopstix
    cd chopstix
    git submodule sync 
    git submodule update --init --recursive

If you download the repository contents directly from `github.com` as a 
compressed zip file, you also have to download the external depencies
and decompress them into the `./external` directory. 

### Compilation

ChopStiX uses [CMake] as build system. We have provided a simple wrapper
in form of a configure script in order to provide a more accessible interface.

The basic build workflow is as follows:

    mkdir build
    cd build
    ../configure
    make && make install

For more detailed information regarding configuration options see 
[installation](docs/installation.md) documentation.

## Basic usage

ChopStiX saves all collected information in a local SQL database.
By default it will save data to `chop.db`. Most commands have a `-data`
option to change this path.

In general, you can invoke any command using `chop <command>`.
For more information about a specific command, try `chop help <command>`.
There are also some utility scripts (i.e. `chop-marks`) which are generally
prefixed by `chop-`.

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
