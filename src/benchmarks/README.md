# benchmarks

This directory contains definitions of benchmarks for the different transfer mechanisms.
To add a new benchmark (see also bench_dummy.c):
1. define a struct for the data that is used throughout a benchmark of the transfer mechanism
2. define functions for the individual steps of a transfer (initialization, starting, sending partition, receiving partition, completion and cleanup). Reuse existing functions from this directory if possible.
3. define an instance of the benchmarking_function struct, with the size of the struct from (1.) and pointers to the functions from (2.)
4. define the benchmarking entry point, i.e a function matching the signature of the bench-functions in test_cases.h

The benchmark can then be included in test_cases.h.