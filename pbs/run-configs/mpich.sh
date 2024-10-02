#!/bin/bash
source ./pbs/run-configs.sh

MPI_RUN=mpirun
MPI_ARGS=-np 2 -ppn 1
EXECUTABLE=$(bin_directory)/bench_mpich

IMPL=mpich/4.1.2

module load hlrs-software-stack/current gcc/13.1.0
module load mpich/4.1.2
