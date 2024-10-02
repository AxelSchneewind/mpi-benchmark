### FOR LOCAL TESTING
MPI_DIR=/home/axel/software/openmpi-5.0.0/build/bin/
# MPI_DIR=/home/axel/software/mpich-4.2.0rc1/build/bin/

MPI_RUN=$(MPI_DIR)mpirun
MPI_CC=$(MPI_DIR)mpicc


.phony: all run debug deploy get put clean
all: bench

# remove build files
clean: 
	rm -rf build/*

MPI_CC_FLAGS=-Isrc -lm -lpthread -Wpedantic -fopenmp

# benchmark compilation units
MAIN_SRC=src/bench.c
MAIN_OBJ=$(patsubst src/%.c,build/%.o, $(MAIN_SRC))
FRAMEWORK_SRC=$(filter-out src/bench.c src/custom_psend_dummy.c, $(wildcard src/*.c)) experiments/message-aggregation/custom_psend_aggregate_simple.c
FRAMEWORK_OBJ=$(patsubst src/%.c,build/%.o, $(FRAMEWORK_SRC))
BENCHMARKS_SRC=$(wildcard src/benchmarks/*.c)
BENCHMARKS_OBJ=$(patsubst src/%.c,build/%.o, $(BENCHMARKS_SRC))

# compile benchmarks and framework into individual object files
build/%.o: src/%.c
	mkdir -p $(@D) &&\
	$(MPI_CC) $(filter %.c %.o,$^) -O2 -c -Isrc -fopenmp -o $@ 

# command line argument parsing
src/cmdline.c: src/cmdline.ggo
	gengetopt -i src/cmdline.ggo --output-dir=src


# executables are compiled here 
bench_dbg: $(MAIN_SRC) $(BENCHMARKS_OBJ) $(FRAMEWORK_OBJ)
	$(MPI_CC) $(filter %.c %.o,$^) -o $@ $(MPI_CC_FLAGS) -g

bench: $(MAIN_SRC) $(BENCHMARKS_OBJ) $(FRAMEWORK_OBJ)
	$(MPI_CC) $(filter %.c %.o,$^) -o $@ $(MPI_CC_FLAGS) -O2 -DNDEBUG


# arguments for running benchmarks
BENCH_BUFFER_SIZE=23
BENCH_ITERATION_COUNT=100
BENCH_WARMUP_ITERATIONS=10
BENCH_MODES=Send,SendPersistent,Isend,Psend,IsendTest,PsendProgress,PsendCustom
BENCH_MIN_THREAD_COUNTS=0
BENCH_MAX_THREAD_COUNTS=4
BENCH_MIN_PARTITION_SIZES=12
BENCH_MAX_PARTITION_SIZES=23
BENCH_SEND_PATTERNS=Linear,LinearInverse,GridBoundary
BENCH_ARGS=-b $(BENCH_BUFFER_SIZE) -i $(BENCH_ITERATION_COUNT) -I $(BENCH_WARMUP_ITERATIONS) -m $(BENCH_MODES) -t $(BENCH_MIN_THREAD_COUNTS) -T $(BENCH_MAX_THREAD_COUNTS) -p $(BENCH_MIN_PARTITION_SIZES) -P $(BENCH_MAX_PARTITION_SIZES) -s $(BENCH_SEND_PATTERNS) -o"out0.csv","out1.csv"

run: bench
	$(MPI_RUN) $(MPI_RUN_OPTIONS) -n 2 ./bench $(BENCH_ARGS) 

run_valgrind: bench_dbg
	$(MPI_RUN) -n 2 valgrind --leak-check=yes --log-file=valgrind-%p.txt ./bench_dbg $(BENCH_ARGS)
# --suppressions=$(MPI_DIR)/../share/openmpi/openmpi-valgrind.supp 

debug: bench_dbg
	$(MPI_RUN) $(MPI_RUN_OPTIONS) -n 2 ddd --args ./bench_dbg $(BENCH_ARGS)

put:
	scp -r pbs benchmarks *.c *.h message-aggregation hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark
	scp Makefile_hawk hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/Makefile

get: 
	rm -rf results/
	scp -r hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/results .
	scp hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/Makefile Makefile_hawk
