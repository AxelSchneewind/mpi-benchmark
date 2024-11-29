#!/bin/bash
source $PBS_O_WORKDIR/pbs/run-configs.sh

MPI_RUN=mpirun
# TODO check what prte_allocation does
MPI_ARGS=-np 2 -N 1 --mca btl ^uct --mca pml ob1 --mca prte_allocation_required 0
EXECUTABLE=$PBS_O_WORKDIR/build/ompi/bench

IMPL=openmpi/5.0.3/ob1
module load hlrs-software-stack/current gcc/13.1.0
module load openmpi/5.0.3
