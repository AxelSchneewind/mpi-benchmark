#!/bin/bash
source $PBS_O_WORKDIR/pbs/run-configs.sh

MPI_RUN=mpirun
MPI_ARGS=-np 2 -N 1 --mca btl ^uct --mca pml ucx
EXECUTABLE=$PBS_O_WORKDIR/build/ompi/bench

IMPL=openmpi/5.0.3/ucx
module load hlrs-software-stack/current gcc/13.1.0
module load openmpi/5.0.3
