
### FOR LOCAL TESTING

MPI_DIR=/home/axel/software/openmpi-5.0.0/build/bin/
# MPI_DIR=/home/axel/software/mpich-4.2.0rc1/build/bin/

# arguments for running benchmarks
BENCH_BUFFER_SIZE=18
BENCH_ITERATION_COUNT=100
BENCH_MODES=Send,SendPersistent,Isend,Psend,PsendParrived,PsendProgress
BENCH_MIN_THREAD_COUNTS=0
BENCH_MAX_THREAD_COUNTS=6
BENCH_MIN_PARTITION_SIZES=8
BENCH_MAX_PARTITION_SIZES=23
BENCH_SEND_PATTERNS=0
BENCH_ARGS=-b $(BENCH_BUFFER_SIZE) -i $(BENCH_ITERATION_COUNT) -m $(BENCH_MODES) -t $(BENCH_MIN_THREAD_COUNTS) -T $(BENCH_MAX_THREAD_COUNTS) -p $(BENCH_MIN_PARTITION_SIZES) -P $(BENCH_MAX_PARTITION_SIZES)

SETUP=FULL_LOCAL

MPI_RUN=$(MPI_DIR)mpirun
MPI_CC=$(MPI_DIR)mpicc

.phony: all run debug ddd deploy run-remote get put run_get_status run_parrived
all: bench

cmdline.c: cmdline.ggo
	gengetopt -i cmdline.ggo


SRC=$(wildcard benchmarks/*.c) $(wildcard *.c)


bench_dbg: $(SRC) cmdline.c bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench_dbg -Wall -g -lpthread -I. -lm -fopenmp

bench: $(SRC) cmdline.c bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench -lm -lpthread -I. -Wall -O2 -DNDEBUG -fopenmp


run: bench
	$(MPI_RUN) --mca mpi_param_check 1 --mca mpi_show_handle_leaks 1 -n 2 ./bench $(SETUP) $(BENCH_ARGS)

#$(MPI_RUN) --mca pml ob1 -n 2 ./bench 

run_valgrind: bench
	$(MPI_RUN) -n 2 valgrind --suppressions=$(MPI_DIR)/../share/openmpi/openmpi-valgrind.supp --leak-check=yes --log-file=valgrind-%p.txt ./bench  $(SETUP)


debug: bench_dbg
	$(MPI_RUN) -n 2 ddd --args ./bench_dbg  $(SETUP)

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
	scp Makefile_hawk hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/Makefile

get: 
	rm -rf results/
	scp -r hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/results .
	scp hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/Makefile Makefile_hawk
