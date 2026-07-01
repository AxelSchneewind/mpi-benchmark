
SETUP=PARTITIONED_LOCAL

MPI_RUN=../ompi/build/bin/mpirun
CC=../ompi/build/bin/mpicc

SRC=$(wildcard benchmarks/*.c) $(wildcard *.c)

.phony: all run debug 
all: bench


cmdline.c cmdline.h: cmdline.ggo
	gengetopt -i cmdline.ggo -F cmdline

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
