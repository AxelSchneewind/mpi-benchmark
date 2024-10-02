#!/bin/bash
source ./pbs/run-configs.sh

MPI_RUN=mpirun
MPI_ARGS=-np 2 -N 1 --mca btl ^uct 
EXECUTABLE=$(bin_directory)/bench_openmpi_503

IMPL=openmpi/5.0.3/default
module load hlrs-software-stack/current gcc/13.1.0
module load openmpi/5.0.3