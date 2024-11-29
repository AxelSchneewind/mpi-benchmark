#!/bin/bash
source ./pbs/run-configs.sh

MPI_RUN=mpirun
MPI_ARGS=-np 2
EXECUTABLE=$PBS_O_WORKDIR/build/impi/bench

IMPL=impi/19.1.0

module load hlrs-software-stack/current gcc/13.1.0
module load impi/19.1.0
