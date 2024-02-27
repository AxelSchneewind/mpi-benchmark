
### FOR LOCAL TESTING

#MPI_DIR=/home/axel/software/openmpi-5.0.0/build/bin/
MPI_DIR=/home/axel/software/mpich-4.2.0rc1/build/bin/
SETUP=MPICH_LOCAL

MPI_RUN=$(MPI_DIR)mpirun
MPI_CC=$(MPI_DIR)mpicc

SRC=$(wildcard benchmarks/*.c) $(filter-out interval_tree_test.c get_status.c parrived.c custom_psend_old.c, $(wildcard *.c))

.phony: all run debug ddd deploy run-remote get put run_get_status run_parrived
all: bench

bench_dbg: $(SRC) bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench_dbg -Wall -g -lpthread -I. -lm -fopenmp

bench: $(SRC) bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench -lm -lpthread -I. -Wall -O2 -DNDEBUG -fopenmp


run: bench
	$(MPI_RUN) -n 2 ./bench $(SETUP)

#$(MPI_RUN) --mca pml ob1 -n 2 ./bench 

run_valgrind: bench
	$(MPI_RUN) -n 2 valgrind --suppressions=$(MPI_DIR)/../share/openmpi/openmpi-valgrind.supp --leak-check=yes --log-file=valgrind-%p.txt ./bench  $(SETUP)


debug: bench_dbg
	$(MPI_RUN) -n 2 ddd ./bench_dbg  $(SETUP)

tree_test: send_patterns.c interval_tree.c interval_tree_test.c
	gcc send_patterns.c interval_tree.c interval_tree_test.c -g -o tree_test
run_tree_test: tree_test
	./tree_test

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