#!/bin/bash
source ./pbs/run-configs.sh

MPI_RUN=mpirun
MPI_ARGS=-np 2 -N 1 --mca btl ^uct 
EXECUTABLE=$PBS_O_WORKDIR/build/ompi/trace

IMPL=openmpi/5.0.3/trace
module load hlrs-software-stack/current gcc/13.1.0
module load openmpi/5.0.3