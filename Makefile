
### FOR LOCAL TESTING

MPI_DIR=/home/axel/work/openmpi-development/devel/ompi/build/bin/
SETUP=PARTITIONED_LOCAL

MPI_RUN=$(MPI_DIR)mpirun
CC=$(MPI_DIR)mpicc

SRC=$(wildcard benchmarks/*.c) $(filter-out interval_tree_test.c get_status.c parrived.c custom_psend_old.c custom_psend_new.c partitioned_get_status.c win.c, $(wildcard *.c))

.phony: all run debug ddd deploy run-remote get put run_get_status run_parrived
all: bench

bench_dbg: $(SRC) bench.h test_cases.h
	$(CC) $(SRC) -o bench_dbg -Wall -g -lpthread -I. -lm -fopenmp

bench: $(SRC) bench.h test_cases.h
	$(CC) $(SRC) -o bench -lm -lpthread -I. -Wall -O2 -DNDEBUG -fopenmp


run: bench
	$(MPI_RUN) --mca part direct --mca mpi_param_check 1 --mca mpi_show_handle_leaks 1 -n 2 ./bench $(SETUP)

run_valgrind: bench
	$(MPI_RUN) -n 2 valgrind --suppressions=$(MPI_DIR)/../share/openmpi/openmpi-valgrind.supp --leak-check=yes --log-file=valgrind-%p.txt ./bench  $(SETUP)


debug: bench_dbg
	$(MPI_RUN) -n 2 ddd --args ./bench_dbg  $(SETUP)

