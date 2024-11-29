#!/bin/bash
source $PBS_O_WORKDIR/pbs/run-configs.sh

MPI_RUN=mpirun
MPI_ARGS=-np 2 -ppn 1
EXECUTABLE=$PBS_O_WORKDIR/build/mpich/bench

IMPL=mpich/4.1.2

module load hlrs-software-stack/current gcc/13.1.0
module load mpich/4.1.2
