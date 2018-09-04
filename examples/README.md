# Examples

The examples directory contains simple testbenches for the functions in the HOPS
library. These testbenches demonstrate simple use-cases and complex application kernels.

Each folder contains a test.cpp file implementing main() and a makefile. The
makefile includes include.mk from this directory. The default make rule (`all`)
runs `sw_test` (software test, g++ compilation), and `csim` (Vivado HLS
compilation). Running `make` inside each directory will test functional
correctness in g++ and Vivado HLS.

To test synthesizablility run `make synth`. This will compile, synthesize, and
perform IP Export for the functions enumerated by the FUNCTIONS variable in each
directory's makefile.

## Testbench Directories

These directories test the functions in our HOPS library. These tests are not
meant to be extensive.

| Folder      | Description |
| ----------- |:----------- |
| [**arrayops**](./arrayops) | Tests the functions in the arrayops.hpp library file
| [**functools**](./functools) | Tests the functions in the functools.hpp library file
| [**reduce**](./reduce) | Tests the functions in the reduce.hpp library file
| [**zip**](./zip) | Tests the functions in the zip.hpp library file
| [**map**](./map) | Tests the functions in the map.hpp library file
| [**divconq**](./divconq) | Tests the functions in the divconq.hpp library file

The testbench in each folder demonstrates at least three uses cases: Writing a
wrapped function, re-using a wrapped function from <functional>, and using a
lambda function to wrap an existing implementation.

## Application Examples

The application folders demonstrate application examples from the papers that
influenced this work and were reported in our TCAD paper. They are listed in
order of increasing complexity.

| Folder      | Description |
| ----------- |:----------- |
| [**insertion_sort**](./insertion_sort) | The insertion sort function from *"Resolve: Generation of high-performance sorting architectures from high-level synthesis"* implemented with our higher-order functions
| [**finite_impulse_response**](./finite_impulse_response) | Finite impulse response kernel based on our paper *"A FPGA design for high speed feature extraction from a compressed measurement stream"*
| [**argminmax**](./argminmax) | ArgMin/ArgMax implementation from our paper *"A streaming clustering approach using a heterogeneous system for big data analysis"*
| [**fast_fourier_transform**](./fast_fourier_transform) | An implementation of the Fast-Fourier Transform algorithm. 
| [**bitonic_sort**](./bitonic_sort) | An implementation of a parallel bitonic sorter from *"Resolve: Generation of high-performance sorting architectures from high-level synthesis"* implemented with our higher-order functions
| [**smith_waterman**](./smith_waterman) | An implementation of the Smith-Waterman string-matching algorithm used in *"A Model for Programming Data-Intensive Applications on FPGAs: A Genomics Case Study"*

## Files

This folder contains four files:

* **README.md:** This file.

* **utility.hpp:** Implements several useful testing functions.

* **include.mk** The testing makefile included by the makefile in each
  sub-directory.
  
* **hls.tcl** The Vivado HLS Compile script.




