#include "permutation.h"

#include "benchmarks/bench.h"

#include <stdlib.h>

typedef int (*benchmarking_step)(TestCase *test_case, Result *result, int comm_rank, void* state);
typedef int (*benchmarking_step_partition)(TestCase *test_case, Result *result, int comm_rank, int partition, void* state);

struct benchmarking_function {
    benchmarking_step const init;
    benchmarking_step const start;
    benchmarking_step_partition const partition_operation_send;
    benchmarking_step_partition const partition_operation_recv;
    benchmarking_step const complete;
    benchmarking_step const cleanup;
};

void execute(TestCase *test_case, Result *result, int comm_rank, const struct benchmarking_function* functions_ptr, int state_size) {
{                                                                                                                                           
    const struct benchmarking_function* functions = functions_ptr;                                                                          
    char state_buf[state_size];     /* allocate state on the stack*/                                                                        
    void* state = (void*) state_buf;                                                                                                        
                                                                                                                                            
    functions->init(test_case, result, comm_rank, state);                                                                                   
                                                                                                                                            
    timers timers;                                                                                                                          
    timers_init(&timers, TimerCount);                                                                                                       
                                                                                                                                            
    /* warmup */                                                                                                                            
    for(int it = 0; it < WARMUP_ITERATIONS; it++) {                                                                                         
        functions->start(test_case, result, comm_rank, state);                                                                              
                                                                                                                                            
        if (0 == comm_rank) {                                                                                                               
            #pragma omp parallel for num_threads(test_case->thread_count)                                                                   
            for (int t = 0; t < test_case->thread_count; t++) {                                                                             
                for (int p = 0; p < test_case->partitions_per_thread; p++) {                                                                
                    unsigned int partition_num = *permutation_at(test_case->send_pattern, p + t * test_case->partitions_per_thread);        
                    functions->partition_operation_send(test_case, result, comm_rank, partition_num, state);                                
                }                                                                                                                           
            }                                                                                                                               
        } else {                                                                                                                            
            #pragma omp parallel for num_threads(test_case->thread_count)                                                                   
            for (int t = 0; t < test_case->thread_count; t++) {                                                                             
                for (int p = 0; p < test_case->partitions_per_thread; p++) {                                                                
                    unsigned int partition_num = *permutation_at(test_case->send_pattern, p + t * test_case->partitions_per_thread);        
                    functions->partition_operation_recv(test_case, result, comm_rank, partition_num, state);                                
                }                                                                                                                           
            }                                                                                                                               
        }                                                                                                                                   
                                                                                                                                            
        functions->complete(test_case, result, comm_rank, state);                                                                           
    }                                                                                                                                       
    usleep(POST_WARMUP_SLEEP_US);                                                                                                           
                                                                                                                                            
                                                                                                                                            
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));                                                                                                 
    timers_start(timers, Total);                                                                                                            
                                                                                                                                            
    for (size_t i = 0; i < test_case->iteration_count; i++)                                                                                 
    {                                                                                                                                       
        timers_start(timers, Iteration);                                                                                                    
        timers_start(timers, IterationStartToWait);                                                                                         
                                                                                                                                            
        if (0 == comm_rank) {                                                                                                               
            #pragma omp parallel for num_threads(test_case->thread_count)                                                                   
            for (int t = 0; t < test_case->thread_count; t++) {                                                                             
                for (size_t p = 0; p < test_case->partitions_per_thread; p++) {                                                             
                    unsigned int partition_num = *permutation_at(test_case->send_pattern, p + t * test_case->partitions_per_thread);        
                                                                                                                                            
                    work(test_case->partition_size);                                                                                        
                                                                                                                                            
                    functions->partition_operation_send(test_case, result, comm_rank, partition_num, state);                                
                }                                                                                                                           
            }                                                                                                                               
        } else {                                                                                                                            
            #pragma omp parallel for num_threads(test_case->thread_count)                                                                   
            for (int t = 0; t < test_case->thread_count; t++) {                                                                             
                for (size_t p = 0; p < test_case->partitions_per_thread; p++) {                                                             
                    unsigned int partition_num = *permutation_at(test_case->send_pattern, p + t * test_case->partitions_per_thread);        
                                                                                                                                            
                    work(test_case->partition_size);                                                                                        
                                                                                                                                            
                    functions->partition_operation_recv(test_case, result, comm_rank, partition_num, state);                                
                }                                                                                                                           
            }                                                                                                                               
        }                                                                                                                                   
                                                                                                                                            
        timers_stop(timers, IterationStartToWait);                                                                                          
                                                                                                                                            
        functions->complete(test_case, result, comm_rank, state);                                                                           
                                                                                                                                            
        timers_stop(timers, Iteration);                                                                                                     
    }                                                                                                                                       
                                                                                                                                            
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));                                                                                                 
    timers_stop(timers, Total);                                                                                                             
                                                                                                                                            
    functions->cleanup(test_case, result, comm_rank, state);                                                                                
                                                                                                                                            
    timers_store(timers, result);                                                                                                           
    timers_free(timers);                                                                                                                    
}}                                                                                                                                          

