#!/bin/bash
source $PBS_O_WORKDIR/pbs/run-configs.sh

MPI_RUN=mpirun
MPI_ARGS=-np 2 -N 1
EXECUTABLE=$PBS_O_WORKDIR/build/mpt/bench

IMPL=mpt/2.28

module load hlrs-software-stack/current gcc/10.2.0 mpt/2.28
