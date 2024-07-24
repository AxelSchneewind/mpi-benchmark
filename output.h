#pragma once

#include "test_cases.h"

#include <stdio.h>

const char* result_file_name(int comm_rank);
int result_file_exists(const char* filename);

void result_file_create(const char* filename);
FILE *result_file_open(char** filenames, int filenames_given, int comm_rank);
void result_file_close(FILE *file);

void record_result(TestCase *test_case, Result *result, FILE *file);

