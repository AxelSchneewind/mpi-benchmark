#include "bench.h"
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>


/*
In openMPI on hawk with ucx, crashes with error:
    [r33c2t5n1:2168273:0:2168273] Caught signal 11 (Segmentation fault: address not mapped to object at address 0x1)
    ==== backtrace (tid:2168273) ====
     0 0x0000000000012cf0 __funlockfile()  :0
     1 0x00000000002796be mca_pml_ucx_psend_completion()  ???:0
     2 0x000000000007606d ucp_rndv_ats_handler()  ???:0
     3 0x000000000003f319 uct_rc_mlx5_iface_check_rx_completion()  ???:0
     4 0x000000000004890a ucp_worker_progress()  ???:0
     5 0x00000000000276e3 opal_progress()  ???:0
     6 0x000000000005fb25 ompi_sync_wait_mt()  ???:0
     7 0x000000000009795f ompi_request_default_wait()  ???:0
     8 0x00000000000e8ffe MPI_Wait()  ???:0
     9 0x0000000000403cbb bench_psend_progress_thread()  ???:0
    10 0x0000000000406931 bench()  ???:0
    11 0x0000000000401905 main()  ???:0
    12 0x000000000003ad85 __libc_start_main()  ???:0
    13 0x0000000000401c7e _start()  ???:0
    =================================
*/
void bench_psend_progress(TestCase *test_case, Result *result, int comm_rank)
{
    // init request and timer
    MPI_Request request;

    timer *timers;
    timers_init(&timers, test_case, result);

    if (comm_rank == 0)
    {
        MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
    }
    else if (comm_rank == 1)
    {
        MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
    }

    // run
    MPI_Barrier(MPI_COMM_WORLD);
    timers_start_global(timers);

    if (comm_rank == 0)
    {
        for (size_t i = 0; i < test_case->iteration_count; i++)
        {
            timers_start_local(timers);

            // printf("s1\n"); fflush(stdout);
            MPI_Start(&request);

            int flag = false;
            // printf("s2\n"); fflush(stdout);
            for (size_t p = 0; p < test_case->partition_count; p++)
            {
                unsigned int partition_num = test_case->send_pattern[p];

                // trigger progress as MPI_Pready doesn't (at least for openmpi)
                MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE);

                /* how can MPI_Request_get_status return true if not all Pready calls have been made?
                 * only occurs with openmpi and only for i % 2 == 1 
                 */
                // if (flag) {            
                //                        
                //     printf("How did I get here (partition %i of %i, iteration %i)\n", p, test_case->partition_count, i); 
                //     fflush(stdout);
                //    break;
                // }
                MPI_Pready(partition_num, request);
            }

            // printf("s3\n"); fflush(stdout);
            // sometimes gets stuck here, but only if MPI_Request_get_status is called
            MPI_Wait(&request, &result->send_status);
            // printf("done\n");

            timers_stop_local(timers);
        }
    } else if (comm_rank == 1) {
        for (size_t i = 0; i < test_case->iteration_count; i++)
        {
            timers_start_local(timers);

            MPI_Start(&request);
            MPI_Wait(&request, &result->recv_status);

            timers_stop_local(timers);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    timers_stop_global(timers);

    MPI_Request_free(&request);

    timers_store(timers, result);
    timers_free(timers);
};
