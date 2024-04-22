#!/bin/bash

# arguments for running benchmarks
BENCH_BUFFER_SIZE=18
BENCH_ITERATION_COUNT=100
BENCH_MODES=Send,SendPersistent,Isend,Psend,PsendParrived,PsendProgress
BENCH_MIN_THREAD_COUNTS=0
BENCH_MAX_THREAD_COUNTS=6
BENCH_MIN_PARTITION_SIZES=8
BENCH_MAX_PARTITION_SIZES=23
BENCH_SEND_PATTERNS=0

make_bench_args() {
	BENCH_ARGS="-b $BENCH_BUFFER_SIZE -i $BENCH_ITERATION_COUNT -m $BENCH_MODES -t $BENCH_MIN_THREAD_COUNTS -T $BENCH_MAX_THREAD_COUNTS -p $BENCH_MIN_PARTITION_SIZES -P $BENCH_MAX_PARTITION_SIZES"
	echo "$BENCH_ARGS"
}

setup() {
	NAME="$1"
	cd $PBS_O_WORKDIR

	mkdir -p results/"$NAME"/
	rm -f results/"$NAME"/*
	cd results/"$NAME"

	cat $PBS_NODE_FILE > node-file.txt
}

bin_directory() {
	echo "$PBS_O_WORKDIR/bin/"
}
