# HOPS: Higher-Order Functions (Patterns) for C++ Hardware Synthesis

Higher-order functions represent common algorithmic patterns: Function
Composition, Reduction Trees, List Traversals, and Networks. However,
traditional software implementations are not _synthesizable_; they rely on
dynamic runtime features that are not supported when compiling static hardware
circuits. Consequently, Higher-Order Functions are not available in C++
synthesis toolchains that are used to implement hardware circuits.

This repository demonstrates how Higher-Order Functions can be provided using a
techinque known as C++ Template Metaprogramming that provides runtime-like
(dynamic) features at compile time (statically). C++ Metaprogramming is
traditionally considered complex and overly verbose. Fortunately, new features in
the C++ language spec have simplified this complexity.

HOPS is a library of Higher-Order Functions for C++ Hardare Synthesis tools. In
this repository we have provided functions, testbenches, and application
examples demonstrating how to use our higher-order functions. We have tested
these functions in Vivado 2017.4-2018.2. 

## Directory Structure

* **include** --- Our library of Higher-Order Functions. (See this directory for
  function descriptions).
* **examples** --- Tests and application examples using our Higher-Order Functions
* **docs** --- Document directory, which includes our paper at CODES 2018 on this
work.

## Using our Library

To use our library, add the **include/** directory to your compiler include path
with the -I flag. You will also need to use the -std=c++11 flag.

## License

This work is released under a BSD-3 License. 

## Version Log

* **Version 1.0**: Initial release. Fully-Parallel (Unrolled) Higher-Order
functions.

