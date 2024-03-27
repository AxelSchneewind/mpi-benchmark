

// could enable Barrier on each iteration here
#define PreIteration { /*MPI_Barrier(MPI_COMM_WORLD);*/ }



/* #define TEST(name, initialization, warmup, partition_send, partition_recv) void name(TestCase *test_case, Result *result, int comm_rank) {\
     initialization;
     warmup;
 
 	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
 	timers_start(timers, Total);
 
 
     if (0 == comm_rank) {
 	    for (size_t i = 0; i < test_case->iteration_count; i++) {
 			unsigned int partition_num = test_case->send_pattern[p];
             partition_send;
         }
     } else if (1 == comm_rank) {
 	    for (size_t i = 0; i < test_case->iteration_count; i++) {
 			unsigned int partition_num = test_case->recv_pattern[p];
             partition_recv;
         }
     }
 
 
 	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
 	timers_stop(timers, Total);
 
 	timers_store(timers, result);
 	timers_free(timers);
 }*/