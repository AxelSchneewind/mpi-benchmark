#!/bin/bash

# arguments for running benchmarks
make_bench_args() {
	local BENCH_OUTPUT_FILES="R0.csv,R1.csv"
	local BENCH_BUFFER_SIZE=23
	local BENCH_ITERATION_COUNT=100
	local BENCH_MODES=Send,SendPersistent,Isend,Psend
	local BENCH_MIN_THREAD_COUNTS=0
	local BENCH_MAX_THREAD_COUNTS=4
	local BENCH_MIN_PARTITION_SIZES=12
	local BENCH_MAX_PARTITION_SIZES=23
	local BENCH_SEND_PATTERNS=Linear,Stride2,Random,RandomBurst1K,GridBoundary
	local BENCH_ARGS="-b $BENCH_BUFFER_SIZE -i $BENCH_ITERATION_COUNT -m $BENCH_MODES -t $BENCH_MIN_THREAD_COUNTS -T $BENCH_MAX_THREAD_COUNTS -p $BENCH_MIN_PARTITION_SIZES -P $BENCH_MAX_PARTITION_SIZES -s $BENCH_SEND_PATTERNS -o $BENCH_OUTPUT_FILES"
	echo "$BENCH_ARGS"
}


# generates cmdline arguments to pass to benchmark executable for tracing
make_trace_args() {
	local TRACE_BUFFER_SIZE=23
	local TRACE_ITERATION_COUNT=1
	local TRACE_MODES=Send,SendPersistent,Isend,Psend,PsendProgress
	local TRACE_MIN_THREAD_COUNTS=0
	local TRACE_MAX_THREAD_COUNTS=3
	local TRACE_MIN_PARTITION_SIZES=17
	local TRACE_MAX_PARTITION_SIZES=17
	local TRACE_SEND_PATTERNS=0
	local TRACE_ARGS="-b $TRACE_BUFFER_SIZE -i $TRACE_ITERATION_COUNT -m $TRACE_MODES -t $TRACE_MIN_THREAD_COUNTS -T $TRACE_MAX_THREAD_COUNTS -p $TRACE_MIN_PARTITION_SIZES -P $TRACE_MAX_PARTITION_SIZES -s $TRACE_SEND_PATTERNS"
	echo "$TRACE_ARGS"
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
