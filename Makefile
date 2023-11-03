
MPI_DIR=/home/axel/software/openmpi-5.0.0/build/bin/
#MPI_DIR=/home/axel/software/mpich-4.1.2/build/bin/

MPI_RUN=$(MPI_DIR)mpirun
MPI_CC=$(MPI_DIR)mpicc

SRC=$(wildcard benchmarks/*.c) $(wildcard *.c)

.phony: all run debug ddd deploy run-remote get put
all: bench

bench_dbg: $(SRC) bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench_dbg -Wall -g -lpthread -I.

bench: $(SRC) bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench -O3 -lpthread -I. # -Wall


run: bench
	$(MPI_RUN) -n 2 ./bench

debug: bench_dbg
	$(MPI_RUN) -n 2 ./bench_dbg

put:
	scp -r benchmarks *.c *.h *.pbs hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark

get: 
	scp -r hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/results .
