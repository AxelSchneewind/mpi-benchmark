#pragma once


#include "permutation.h"

#include "benchmarks/bench.h"

#include <stdlib.h>

typedef int (*benchmarking_step)(TestCase *testcase, Result *result, int rank, void* state);
typedef int (*benchmarking_step_partition)(TestCase *testcase, Result *result, int rank, int partition, int threadnum, void* state);

struct benchmarking_function {
    int const state_size;
    benchmarking_step const init;
    benchmarking_step const start;
    benchmarking_step_partition const partition_operation_send;
    benchmarking_step_partition const partition_operation_recv;
    benchmarking_step const complete;
    benchmarking_step const cleanup;
};


static void execute(TestCase* testcase, Result* result, int rank, const struct benchmarking_function functions) {                                                 
    char state_buf[functions.state_size];     /* allocate state on the stack*/
    void* state = (void*) state_buf;
                                                                                                                                     
    functions.init(testcase, result, rank, state);
                                                                                                                                     
    timers timers;                                                                                                                   
    timers_init(&timers, TimerCount);                                                                                                
                                                                                                                                     
    /* warmup */                                                                                                                     
    for(int it = 0; it < testcase->warmup_iterations; it++) {                                                                                  
        functions.start(testcase, result, rank, state);                                                                        
                                                                                                                                     
        if (0 == rank) {                                                                                                        
            #pragma omp parallel for num_threads(testcase->thread_count)
            for (int t = 0; t < testcase->thread_count; t++) {                                                                      
                for (int p = 0; p < testcase->partitions_per_thread; p++) {                                                         
                    int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);          
                    functions.partition_operation_send(testcase, result, rank, partition_num, t, state);                          
                }                                                                                                                    
            }                                                                                                                        
        } else {                                                                                                                     
            #pragma omp parallel for num_threads(testcase->thread_count)
            for (int t = 0; t < testcase->thread_count; t++) {                                                                      
                for (int p = 0; p < testcase->partitions_per_thread; p++) {                                                         
                    int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);          
                    functions.partition_operation_recv(testcase, result, rank, partition_num, t, state);                          
                }                                                                                                                    
            }                                                                                                                        
        }                                                                                                                            
                                                                                                                                     
        functions.complete(testcase, result, rank, state);                                                                     
    }                                                                                                                                
    usleep(POST_WARMUP_SLEEP_US);                                                                                                    
                                                                                                                                     
                                                                                                                                     
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));                                                                                          
    timers_start(timers, Total);                                                                                                     
                                                                                                                                     
    for (size_t i = 0; i < testcase->iteration_count; i++)                                                                          
    {                                                                                                                                
        timers_start(timers, Iteration);                                                                                             
        timers_start(timers, IterationStartToWait);                                                                                  

        functions.start(testcase, result, rank, state);                                                                        
                                                                                                                                     
        if (0 == rank) {                                                                                                        
            #pragma omp parallel for num_threads(testcase->thread_count)
            for (int t = 0; t < testcase->thread_count; t++) {                                                                      
                for (size_t p = 0; p < testcase->partitions_per_thread; p++) {                                                      
                    unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread); 
                                                                                                                                     
                    work(testcase->partition_size);                                                                                 
                                                                                                                                     
                    functions.partition_operation_send(testcase, result, rank, partition_num, t, state);                          
                }                                                                                                                    
            }                                                                                                                        
        } else {                                                                                                                     
            #pragma omp parallel for num_threads(testcase->thread_count)                                                            
            for (int t = 0; t < testcase->thread_count; t++) {                                                                      
                for (size_t p = 0; p < testcase->partitions_per_thread; p++) {                                                      
                    unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread); 
                                                                                                                                     
                    work(testcase->partition_size);                                                                                 
                                                                                                                                     
                    functions.partition_operation_recv(testcase, result, rank, partition_num, t, state);                          
                }                                                                                                                    
            }                                                                                                                        
        }                                                                                                                            
                                                                                                                                     
        timers_stop(timers, IterationStartToWait);                                                                                   
                                                                                                                                     
        functions.complete(testcase, result, rank, state);                                                                     
                                                                                                                                     
        timers_stop(timers, Iteration);                                                                                              
    }                                                                                                                                
                                                                                                                                     
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));                                                                                          
    timers_stop(timers, Total);                                                                                                      
                                                                                                                                     
    functions.cleanup(testcase, result, rank, state);                                                                          
                                                                                                                                     
    timers_store(timers, result);                                                                                                    
    timers_free(timers);                                                                                                             
}




// TODO: use target offloading
static void execute_gpu(TestCase* testcase, Result* result, int rank, const struct benchmarking_function functions) {                                                 
    char state_buf[functions.state_size];     /* allocate state on the stack*/
    void* state = (void*) state_buf;
                                                                                                                                     
    functions.init(testcase, result, rank, state);
                                                                                                                                     
    timers timers;                                                                                                                   
    timers_init(&timers, TimerCount);                                                                                                
                                                                                                                                     
    /* warmup */                                                                                                                     
    for(int it = 0; it < WARMUP_ITERATIONS; it++) {                                                                                  
        functions.start(testcase, result, rank, state);                                                                        
                                                                                                                                     
        if (0 == rank) {                                                                                                        
            #pragma omp parallel for num_threads(testcase->thread_count)
            for (int t = 0; t < testcase->thread_count; t++) {                                                                      
                for (int p = 0; p < testcase->partitions_per_thread; p++) {                                                         
                    int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);          
                    functions.partition_operation_send(testcase, result, rank, partition_num, t, state);                          
                }                                                                                                                    
            }                                                                                                                        
        } else {                                                                                                                     
            #pragma omp parallel for num_threads(testcase->thread_count)
            for (int t = 0; t < testcase->thread_count; t++) {                                                                      
                for (int p = 0; p < testcase->partitions_per_thread; p++) {                                                         
                    int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);          
                    functions.partition_operation_recv(testcase, result, rank, partition_num, t, state);                          
                }                                                                                                                    
            }                                                                                                                        
        }                                                                                                                            
                                                                                                                                     
        functions.complete(testcase, result, rank, state);                                                                     
    }                                                                                                                                
    usleep(POST_WARMUP_SLEEP_US);                                                                                                    
                                                                                                                                     
                                                                                                                                     
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));                                                                                          
    timers_start(timers, Total);                                                                                                     
                                                                                                                                     
    for (size_t i = 0; i < testcase->iteration_count; i++)                                                                          
    {                                                                                                                                
        timers_start(timers, Iteration);                                                                                             
        timers_start(timers, IterationStartToWait);                                                                                  
                                                                                                                                     
        if (0 == rank) {                                                                                                        
            #pragma omp target teams distribute parallel for num_threads(testcase->thread_count)
            for (int t = 0; t < testcase->thread_count; t++) {                                                                      
                for (size_t p = 0; p < testcase->partitions_per_thread; p++) {                                                      
                    unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread); 
                                                                                                                                     
                    work(testcase->partition_size);                                                                                 
                                                                                                                                     
                    functions.partition_operation_send(testcase, result, rank, partition_num, t, state);                          
                }                                                                                                                    
            }                                                                                                                        
        } else {                                                                                                                     
            #pragma omp target teams distribute parallel for num_threads(testcase->thread_count)                                                            
            for (int t = 0; t < testcase->thread_count; t++) {                                                                      
                for (size_t p = 0; p < testcase->partitions_per_thread; p++) {                                                      
                    unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread); 
                                                                                                                                     
                    work(testcase->partition_size);                                                                                 
                                                                                                                                     
                    functions.partition_operation_recv(testcase, result, rank, partition_num, t, state);                          
                }                                                                                                                    
            }                                                                                                                        
        }                                                                                                                            
                                                                                                                                     
        timers_stop(timers, IterationStartToWait);                                                                                   
                                                                                                                                     
        functions.complete(testcase, result, rank, state);                                                                     
                                                                                                                                     
        timers_stop(timers, Iteration);                                                                                              
    }                                                                                                                                
                                                                                                                                     
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));                                                                                          
    timers_stop(timers, Total);                                                                                                      
                                                                                                                                     
    functions.cleanup(testcase, result, rank, state);                                                                          
                                                                                                                                     
    timers_store(timers, result);                                                                                                    
    timers_free(timers);                                                                                                             
}