# mpmcplusplus

`mpmcplusplus` is a header-only C++11 library that provides a thread-safe queue container with an easy-to-use interface.

## Table of Contents

- [Goals](#goals)
- [Usage](#usage)
- [Testing](#testing)
- [Documentation](#documentation)
- [Dependencies](#dependencies)

## Goals

This project aims to be a dead-simple, thread-safe implementation of [std::queue](https://en.cppreference.com/w/cpp/container/queue) that allow for communication between consumers and producers in multi producer multi consumer problems.

## Usage

To use `mpmcplusplus` in your project, simply place `mpmcplusplus.h` in your project directory and `#include` it wherever necessary.

## Testing

Testing multi-threaded code is difficult, though I believe I have done a relatively good job testing at least the more common use-cases of this project.

To build and run the tests, execute the below commands:

```shell
$ git clone https://github.com/JTriantafylos/mpmcplusplus.git
$ cd mpmcplusplus
$ cmake -B build -D BUILD_TESTS=ON
$ make -C build test_mpmcplusplus
$ cd bin
$ ./test_mpmcplusplus
```

## Documentation

Documentation is handled via [Doxygen](https://github.com/doxygen/doxygen), meaning you must have Doxygen installed on your system to generate the documentation.

To generate the documentation, execute the below commands:

```shell
$ git clone https://github.com/JTriantafylos/mpmcplusplus.git
$ cd mpmcplusplus
$ cmake -B build -D BUILD_DOCS=ON
$ make -C build doc_mpmcplusplus
```

After running the above commands, the generated documentation will be placed in the `doc` sub-directory of the project.

Doxygen will generate documentation only in the HTML format by default. The flag `-D DOXYGEN_GENERATE_<FORMAT>` can be passed to CMake, where `<FORMAT>` is replaced by a Doxygen supported format, to generate documentation in that format.

## Dependencies

- [doctest](https://github.com/onqtam/doctest) (Tests only)
