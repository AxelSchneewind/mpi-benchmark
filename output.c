#include "output.h"
#include "bench.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>


// where each rank should write its results
char* result_file_paths[2] = {
    "./R0.csv",
    "./R1.csv"
};
 
const char* result_file_name(int comm_rank) {
    if (comm_rank < sizeof(result_file_paths) / sizeof(char*)) {
        return result_file_paths[comm_rank];
    } else {
        return NULL;
    }
}

int result_file_exists(const char* filename) {
    if (NULL != filename) {        // check for file existence
        return (-1 != access(filename, F_OK));
    } else {
        return 0;
    }
}

void result_file_create(const char* filename) {
    FILE *file;

    // open file and overwrite with header line
    file = fopen(filename, "w");

    fprintf(file, "name,run,mode,buffer_size,thread_count,partition_size,partition_size_recv,send_pattern,t_local,t_start_to_wait,t_wait,t_total,t_wait_relative,bandwidth,std_dev(t_local),std_dev(t_start_to_wait),std_dev(t_total)\n");
    fclose(file);
}


FILE *result_file_open(char** filenames, int filenames_given, int comm_rank)
{
    FILE* file = NULL;
    const char* filename;

    // use default name if no argument given
    if (filenames_given <= comm_rank || NULL == filenames[comm_rank] || 0 == strlen(filenames[comm_rank])) {
	printf("Too few filenames given (%i of %i), using default\n", filenames_given, comm_rank);
        filename = result_file_name(comm_rank);
    } else {
        filename = filenames[comm_rank];
	printf("writing to %s\n", filename);
    }

    // if file does not exist yet, create it and write header
    if (!result_file_exists(filename)) {
        result_file_create(filename);
    }

    // open in append mode
    if (comm_rank == 0 || comm_rank == 1) {
        file = fopen(filename, "a");
    } else {
        printf("no output from this rank\n");
    }

    return file;
}

void record_result(TestCase *test_case, Result *result, FILE *file)
{
    fprintf(file, "%s,%i,%s,%lli,%i,%lli,%lli,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
                    test_case->name,
                    test_case->number,
                    mode_names[test_case->mode], 
                    test_case->buffer_size, 
                    test_case->thread_count,
                    test_case->partition_size, 
                    test_case->partition_size_recv, 
                    send_pattern_identifiers[test_case->send_pattern_num], 
                    result->timings[Iteration], 
                    result->timings[IterationStartToWait], 
                    result->timings[Iteration] - result->timings[IterationStartToWait],
                    result->timings[Total], 
                    (result->timings[Iteration] - result->timings[IterationStartToWait]) / result->timings[Iteration],
                    result->bandwidth, 
                    result->timings_std_dev[Iteration],
                    result->timings_std_dev[IterationStartToWait],
                    result->timings_std_dev[Total]);
    fflush(file);
}

void result_file_close(FILE *file)
{
    fclose(file);
}

