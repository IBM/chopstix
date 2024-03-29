# General guidelines

ChopStiX uses the C++11 standard.
More modern C++ should not be used, or only sparingly when necessary,
in order to provide backwards compatibility.

Although no single coding style is enforced, a single style is encouraged.
The `.clang_format` file in the root directory of the project provides
the encouraged settings for code formatting.
When developing you may also run `make format` to automatically format
header and source files (this assumes you have clang-format installed).

In-source builds are prohibited. Please create an extra directory to build
ChopStiX.

If anything breaks, set CHOPSTIX_DEBUG=1 to enable a backtrace.
For more information on specific questions see the documentation in `docs`.

# Directory layout

    src        ChopStiX source and header files
               Each subdirectory has its own CMakeLists.txt
               and builds a single static library
    test       Test definitions
    external   Path to external dependencies of ChopStiX
    scripts    Collection of utility scripts
    cmake      Additional CMake modules and configuration files
    examples   Examples to show ChopStiX use cases
    docs       Extra documentation and related documents
    tools      Utilities that are installed with ChopStiX

## Source code contributions

All pull requests submitted must have their commits signed. You can
have git do this automatically with `git commit -s`. Alternatively,
add a line like the following to the end of your commit:

```
Signed-off-by: Your Name <your.name@your.email>
```

When submitting patches/pull requests, you agree to the following
Developer Certificate of Origin, i.e., items (a--d):

```
Developer Certificate of Origin
Version 1.1

Copyright (C) 2004, 2006 The Linux Foundation and its contributors.
1 Letterman Drive
Suite D4700
San Francisco, CA, 94129

Everyone is permitted to copy and distribute verbatim copies of this
license document, but changing it is not allowed.

Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the open source license
    indicated in the file; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the same open source license (unless I am
    permitted to submit under a different license), as indicated
    in the file; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it, including my sign-off) is
    maintained indefinitely and may be redistributed consistent with
    this project or the open source license(s) involved.

```


