
### FOR LOCAL TESTING

MPI_DIR=/home/axel/software/openmpi-5.0.0/build/bin/
# MPI_DIR=/home/axel/software/mpich-4.2.0rc1/build/bin/

MPI_RUN=$(MPI_DIR)mpirun
MPI_CC=$(MPI_DIR)mpicc

SRC=$(wildcard benchmarks/*.c) $(filter-out get_status.c parrived.c, $(wildcard *.c))

.phony: all run debug ddd deploy run-remote get put run_get_status run_parrived
all: bench

bench_dbg: $(SRC) bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench_dbg -Wall -g -lpthread -I. -lm

bench: $(SRC) bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench -lm -lpthread -I. -Wall -Og -g -DNDEBUG


run: bench
	$(MPI_RUN) ob1 -n 2 ./bench 

#$(MPI_RUN) --mca pml ob1 -n 2 ./bench 

run_valgrind: bench
	$(MPI_RUN) -n 2 valgrind --suppressions=$(MPI_DIR)/../share/openmpi/openmpi-valgrind.supp --leak-check=yes --log-file=valgrind-%p.txt ./bench 


debug: bench_dbg
	$(MPI_RUN) -n 2 ddd ./bench_dbg 

get_status: get_status.c
	$(MPI_CC) get_status.c -o get_status -g -lm -lpthread -I. -Wall

run_get_status: get_status
	$(MPI_RUN) -n 2 ./get_status 

parrived: parrived.c
	$(MPI_CC) parrived.c -o parrived -g -lm -lpthread -I. -Wall

run_parrived: parrived
	$(MPI_RUN) -n 2 ./parrived

put:
	scp -r pbs benchmarks *.c *.h hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark

get: 
	rm -r results/
	scp -r hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/results .
	scp hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/Makefile Makefile_hawk