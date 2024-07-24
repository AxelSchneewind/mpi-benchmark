
### FOR LOCAL TESTING

MPI_DIR=/home/axel/software/openmpi-5.0.0/build/bin/
# MPI_DIR=/home/axel/software/mpich-4.2.0rc1/build/bin/

# arguments for running benchmarks
BENCH_BUFFER_SIZE=23
BENCH_ITERATION_COUNT=1
BENCH_MODES=Send,SendPersistent,Isend,Psend,PsendParrived,PsendProgress# ,PsendCustom
#BENCH_MODES=PsendCustom
BENCH_MIN_THREAD_COUNTS=0
BENCH_MAX_THREAD_COUNTS=4
BENCH_MIN_PARTITION_SIZES=8
BENCH_MAX_PARTITION_SIZES=23
BENCH_SEND_PATTERNS=Linear,LinearInverse,GridBoundary
BENCH_ARGS=-b $(BENCH_BUFFER_SIZE) -i $(BENCH_ITERATION_COUNT) -m $(BENCH_MODES) -t $(BENCH_MIN_THREAD_COUNTS) -T $(BENCH_MAX_THREAD_COUNTS) -p $(BENCH_MIN_PARTITION_SIZES) -P $(BENCH_MAX_PARTITION_SIZES) -s $(BENCH_SEND_PATTERNS) -o out0.csv,out1.csv

SETUP=FULL_LOCAL

MPI_RUN=$(MPI_DIR)mpirun
MPI_CC=$(MPI_DIR)mpicc

.phony: all run debug ddd deploy run-remote get put run_get_status run_parrived
all: bench

cmdline.c: cmdline.ggo
	gengetopt -i cmdline.ggo


SRC=$(wildcard benchmarks/*.c) $(filter-out custom_psend_dummy.c, $(wildcard *.c)) message-aggregation/custom_psend.c message-aggregation/intervals.c


bench_dbg: $(SRC) cmdline.c bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench_dbg -lm -lpthread -I. -Wall -g -fopenmp

bench: $(SRC) cmdline.c bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench -lm -lpthread -I. -Wall -O2 -fopenmp
# -DNDEBUG


run: bench
	$(MPI_RUN) --mca mpi_param_check 1 --mca mpi_show_handle_leaks 1 -n 2 ./bench $(BENCH_ARGS) 
#2> /dev/null

#$(MPI_RUN) --mca pml ob1 -n 2 ./bench 

run_valgrind: bench_dbg
	$(MPI_RUN) -n 2 valgrind --leak-check=yes --log-file=valgrind-%p.txt ./bench_dbg $(BENCH_ARGS)
# --suppressions=$(MPI_DIR)/../share/openmpi/openmpi-valgrind.supp 


debug: bench_dbg
	$(MPI_RUN) -n 2 ddd --args ./bench_dbg $(BENCH_ARGS)

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
	scp -r pbs benchmarks *.c *.h message-aggregation hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark
	scp Makefile_hawk hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/Makefile

get: 
	rm -rf results/
	scp -r hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/results .
	scp hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/Makefile Makefile_hawk
