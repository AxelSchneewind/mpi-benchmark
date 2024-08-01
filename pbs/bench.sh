#!/bin/bash

# arguments for running benchmarks
make_bench_args() {
	local BENCH_NAME="general"
	local BENCH_OUTPUT_FILES="R0.csv,R1.csv"
	local BENCH_BUFFER_SIZE=23
	local BENCH_ITERATION_COUNT=100
	local BENCH_MODES=Send,SendPersistent,SendPersistentTest,Isend,Psend,IsendTest,PsendParrived
	local BENCH_MIN_THREAD_COUNTS=0
	local BENCH_MAX_THREAD_COUNTS=4
	local BENCH_MIN_PARTITION_SIZES=9
	local BENCH_MAX_PARTITION_SIZES=23
	local BENCH_DIFFERENT_PARTITIONINGS='' 		# use -c to enable
	local BENCH_SEND_PATTERNS=Linear,Stride2,Random,RandomBurst1K,GridBoundary
	echo "--bench-name "$BENCH_NAME" -b $BENCH_BUFFER_SIZE -i $BENCH_ITERATION_COUNT -m $BENCH_MODES -t $BENCH_MIN_THREAD_COUNTS -T $BENCH_MAX_THREAD_COUNTS -p $BENCH_MIN_PARTITION_SIZES -P $BENCH_MAX_PARTITION_SIZES -s $BENCH_SEND_PATTERNS -o $BENCH_OUTPUT_FILES $BENCH_DIFFERENT_PARTITIONINGS"
}

make_bench_partitionings_args() {
	local BENCH_NAME="psend_detailed"
	local BENCH_OUTPUT_FILES="R0_psend.csv,R1_psend.csv"
	local BENCH_BUFFER_SIZE=23
	local BENCH_ITERATION_COUNT=100
	local BENCH_MODES=Psend,PsendCustom
	local BENCH_MIN_THREAD_COUNTS=0
	local BENCH_MAX_THREAD_COUNTS=4
	local BENCH_MIN_PARTITION_SIZES=9
	local BENCH_MAX_PARTITION_SIZES=23
	local BENCH_DIFFERENT_PARTITIONINGS='-c' 		# use -c to enable
	local BENCH_SEND_PATTERNS=Linear,Stride2,Random,RandomBurst1K,GridBoundary
	echo "--bench-name "$BENCH_NAME" -b $BENCH_BUFFER_SIZE -i $BENCH_ITERATION_COUNT -m $BENCH_MODES -t $BENCH_MIN_THREAD_COUNTS -T $BENCH_MAX_THREAD_COUNTS -p $BENCH_MIN_PARTITION_SIZES -P $BENCH_MAX_PARTITION_SIZES -s $BENCH_SEND_PATTERNS -o $BENCH_OUTPUT_FILES $BENCH_DIFFERENT_PARTITIONINGS"
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
	local TRACE_SEND_PATTERNS=Linear
	local TRACE_DIFFERENT_PARTITIONINGS='' 		# use -c to enable
	echo "-b $TRACE_BUFFER_SIZE -i $TRACE_ITERATION_COUNT -m $TRACE_MODES -t $TRACE_MIN_THREAD_COUNTS -T $TRACE_MAX_THREAD_COUNTS -p $TRACE_MIN_PARTITION_SIZES -P $TRACE_MAX_PARTITION_SIZES -s $TRACE_SEND_PATTERNS $TRACE_DIFFERENT_PARTITIONINGS"
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
