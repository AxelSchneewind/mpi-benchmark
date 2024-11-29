# ompi
module load gcc/13.1.0 openmpi/5.0.3
make MPI_RUN=mpirun MPI_CC=mpicc BUILD_DIR=build/ompi compile

# custom ompi
module load gcc/13.1.0
CUSTOM_MPI_BIN=$(cd .. && pwd)/ompi/build/bin
make MPI_RUN=$CUSTOM_MPI_BIN/mpirun MPI_CC=$CUSTOM_MPI_BIN/ompi/build/bin/mpicc BUILD_DIR=build/custom build

# mpich
module load gcc/13.1.0 mpich/4.1.2
make MPI_RUN=mpirun MPI_CC=mpicc BUILD_DIR=build/mpich compile

make BENCH_SCRIPT=pbs/bench_openmpi.pbs MPI_RUN=mpirun MPI_CC=mpicc BUILD_DIR=build/ompi run
make BENCH_SCRIPT=pbs/bench_openmpi_aggregation.pbs MPI_RUN=$CUSTOM_MPI_BIN/mpirun MPI_CC=$CUSTOM_MPI_BIN/ompi/build/bin/mpicc BUILD_DIR=build/custom run
make BENCH_SCRIPT=pbs/bench_mpich.pbs MPI_RUN=mpirun MPI_CC=mpicc BUILD_DIR=build/mpich run