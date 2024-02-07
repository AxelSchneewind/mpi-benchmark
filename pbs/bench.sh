#!/bin/bash

setup() {
	NAME="$1"
	cd $PBS_O_WORKDIR

	mkdir -p results/"$NAME"/
	rm -f results/"$NAME"/*
	cd results/"$NAME"/
}

bin_directory() {
	echo "$PBS_O_WORKDIR/bin/"
}
