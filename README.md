**Goals**

* Implement general memory lib routines (aka `malloc, calloc, realloc, ...`).

## Supported platforms

* Linux
* OS X

## Quick Start

* clone repo:

    ```sh
    $ git clone git@github.com:dshil/mm.git
    ```

* install dependencies (WIP)

    ```sh
    $ dnf install -y gcc-c++
    $ dnf install -y wget git
    $ dnf install -y make cmake
    ```

* build and check (Valgrind WIP)

    ```sh
    $ scripts/3rdparty.sh
    $ mkdir build && cd build && cmake .. && make
    ```

* run tests and examples:

    ```sh
    $ bin/test_main
    $ bin/examples
    ```

## Licensing

All code in a public domain.

## Work in progress!
