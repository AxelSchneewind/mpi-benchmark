#!/bin/bash
source $PBS_O_WORKDIR/pbs/run-configs.sh

MPI_RUN=/zhome/academic/HLRS/hlrs/hpcschne/ompi/build/bin/mpirun
MPI_ARGS="-np 2 -N 1 --mca btl ^uct"
EXECUTABLE=$PBS_O_WORKDIR/build/custom/bench

IMPL=openmpi/5.0.3/custom
