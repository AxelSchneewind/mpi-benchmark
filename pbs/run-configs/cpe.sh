#!/bin/bash
source ./pbs/run-configs.sh

MPI_RUN=mpirun
MPI_ARGS=-np 2
EXECUTABLE=$PBS_O_WORKDIR/build/cpe/bench

IMPL=cpe/23.12
module load gcc/10.2.0 cpe/23.12