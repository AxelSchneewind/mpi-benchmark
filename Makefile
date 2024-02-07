
### FOR LOCAL TESTING

MPI_DIR=/home/axel/software/openmpi-5.0.0/build/bin/
#MPI_DIR=/home/axel/software/mpich-4.2.0rc1/build/bin/

MPI_RUN=$(MPI_DIR)mpirun
MPI_CC=$(MPI_DIR)mpicc

SRC=$(wildcard benchmarks/*.c) $(filter-out get_status.c parrived.c, $(wildcard *.c))

.phony: all run debug ddd deploy run-remote get put run_get_status run_parrived
all: bench

bench_dbg: $(SRC) bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench_dbg -Wall -g -lpthread -I. -lm

bench: $(SRC) bench.h test_cases.h
	$(MPI_CC) $(SRC) -o bench -lm -lpthread -I. -Wall
#-O3


run: bench
	$(MPI_RUN) -n 2 ./bench 

#$(MPI_RUN) --mca pml ob1 -n 2 ./bench 

debug: bench_dbg
	$(MPI_RUN) -n 2 ./bench_dbg 

get_status: get_status.c
	$(MPI_CC) get_status.c -o get_status -g -lm -lpthread -I. -Wall

run_get_status: get_status
	$(MPI_RUN) -n 2 ./get_status 

parrived: parrived.c
	$(MPI_CC) parrived.c -o parrived -g -lm -lpthread -I. -Wall

run_parrived: parrived
	$(MPI_RUN) -n 2 ./parrived

put:
	scp -r pbs benchmarks *.c *.h *.pbs hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark

get: 
	rm -f results/*/*
	scp -r hawk:/zhome/academic/HLRS/hlrs/hpcschne/benchmark/results .





### FOR HAWK
# MPI_RUN=mpirun
# MPI_CC=mpicc
# 
# SRC=$(wildcard benchmarks/*.c) $(filter-out get_status.c parrived.c, $(wildcard *.c))
# 
# .phony: all bench run debug ddd deploy run-remote get put bin/bench_openmpi bin/bench_mpich bin/bench_impi bin/bench_cpe
# all: bench
# 
# bench_dbg: $(SRC) bench.h test_cases.h
# 	$(MPI_CC) $(SRC) -o bench_dbg -Wall -g -lpthread -I. -lm
# 
# bin/bench_openmpi: $(SRC) bench.h test_cases.h
# 	echo $(shell module load hlrs-software-stack/current gcc/13.1.0 openmpi/5.0.0 && $(MPI_CC) $(SRC) -o bin/bench_openmpi -lm -lpthread -I. -Wall)
# bin/bench_mpich: $(SRC) bench.h test_cases.h
# 	echo $(shell module load hlrs-software-stack/current gcc/13.1.0 mpich/4.1.2 && $(MPI_CC) $(SRC) -o bin/bench_mpich -lm -lpthread -I. -Wall)
# bin/bench_impi: $(SRC) bench.h test_cases.h
# 	echo $(shell module load hlrs-software-stack/current gcc/13.1.0  impi/2021.9.0 &&  $(MPI_CC) $(SRC) -o bin/bench_impi -lm -lpthread -I. -Wall)
# bin/bench_cpe: $(SRC) bench.h test_cases.h
# 	echo $(shell module load cpe/22.11 && $(MPI_CC) $(SRC) -o bin/bench_cpe -lm -lpthread -I. -Wall)
# bin/bench_mpt: $(SRC) bench.h test_cases.h
# 	echo $(shell module load mpt/2.28 && $(MPI_CC) $(SRC) -o bin/bench_mpt -lm -lpthread -I. -Wall)
# 
# bench: bin/bench_openmpi bin/bench_mpich bin/bench_impi bin/bench_cpe
# 
# .phony: run_bench_openmpi run_bench_mpich run_bench_impi run_bench_cpe
# run_bench_openmpi: bin/bench_openmpi
# 	echo $(shell qsub pbs/bench_openmpi.pbs)
# run_bench_mpich: bin/bench_mpich
# 	echo $(shell qsub pbs/bench_mpich.pbs)
# run_bench_impi: bin/bench_impi
# 	echo $(shell qsub pbs/bench_impi.pbs)
# run_bench_cpe: bin/bench_cpe
# 	echo $(shell qsub pbs/bench_cpe.pbs)
# run_bench_mpt: bin/bench_mpt
# 	echo $(shell qsub pbs/bench_mpt.pbs)
# 
# run: run_bench_openmpi run_bench_mpich run_bench_impi run_bench_cpe
# 
# 
# debug: bench_dbg
# 	$(MPI_RUN) -n 2 ./bench_dbg 
