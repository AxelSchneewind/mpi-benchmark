#!/bin/bash

MPI_RUN=mpirun
MPI_ARGS=-np 2 -N 1
EXECUTABLE=$(bin_directory)/bench_mpt

IMPL=mpt/2.28

module load hlrs-software-stack/current gcc/10.2.0 mpt/2.28