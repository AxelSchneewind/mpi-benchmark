# MPI-Benchmarks

## Building
This project can be built with `make` and provides the following targets:
- `bench` builds the executable, the resulting binary is at `./bench`.
- `bench_dbg` builds the executable for debugging, at `./bench_dbg`.
- `run` runs the benchmarks with default configuration
- `run_valgrind` runs the benchmarks with default configuration using valgrind

To run the code via PBS, use the scripts in the `pbs` directory.

## About the code

To add a benchmark:
1. define the benchmark function
2. add it as extern to `bench.h` 
3. add a new value to `enum Mode` in `test_cases.h`
4. add a corresponding string to `mode_names` in `test_cases.h`



## Notes on Score-P
compile with instrumentation via `scorep-mpicc`. Note that scorep has to be configured with the same mpi installation that is used for running. Run instrumented code to profile.

Use `scorep-score -r -c 3 scorep-APP-TIMESTAMP/profile.cubex > scorep-APP-TIMESTAMP/scorep.score` for scoring (summary). This somehow requires the respective mpi-module to be loaded. Info can be used for filtering and setting SCOREP_TOTAL_MEMORY.

To trace: 
```sh
export MPI_SHEPHERD=true
export SCOREP_TOTAL_MEMORY=10MB
export SCOREP_FILTERING_FILE=APP-scorep.filt
export SCOREP_METRIC_PAPI=PAPI_TOT_INS,PAPI_TOT_CYC,PERF_COUNT_HW_STALLED_CYCLES_BACKEND
scalasca -analyze -t mpirun ... ./app_instrumented 
```
(`-t` for tracing and `-s` for profiling).