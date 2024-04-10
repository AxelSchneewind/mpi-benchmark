# TODO:
- order of Recv-calls: same as send or linear sweep?

- ucx: for <= 1KB = 2^10 message, network card progress

- MPI_Barrier() at begin of each iteration ?

-  "   Another approach is to send one message from every thread as
        soon as the computation is over. This approach is denoted here as
        Pt2Pt many. To avoid competition on the same resource, we first
        duplicate the communicator per thread"
    -> maybe try that too? (duplicating communicators, multithreaded P2P calls)

- RMA: does transfer occur without receive-side get()?
- using one single MPI_Win: one lock/unlock per partition
- single MPI_Win fails if no assertion is passed when locking

- MPICH_ASYNC_PROGRESS does not exist in code


- UCX error
 - [axel-laptop:136610] osc_ucx_component.c:369  Error: OSC UCX component priority set inside component query failed 

# DONE
- find out why ob1 freezes on hawk
    - fix: ```--mca btl ^uct``` works
- use CPE -> cray-mpich-ucx does not implement partitioned communication

## OpenMPI
-   psend_parrived: Parrived() after MPI_Request_get_status(), sometimes completion flag == true and Parrived flag == false
    - sometimes Parrived doesnt return flag==true for all partitions
    - see parrived.c
-   psend_progress: MPI_Request_get_status() returns true before all Pready calls are done, also sometimes gets stuck in MPI_Wait() or MPI_Request_get_status()
    - see get_request.c

# Implementations

## MPICH
- Psend has low bandwidth for all partition sizes
    - counter decremented and buffer send after all partitions ready

## OpenMPI
- partitioned transfer implemented by persistent send/receive per internal partition, using a dedicated communicator
- MPI_Start() exchanges setup information between both ranks
- pready sets flag on partition
- mca_part_persist_progress iterates over all partitions and calls start() for partitions with flag set

# openmpi progress threads
    - OPAL_ENABLE_PROGRESS_THREADS defined (=0) by default
    - functions call opal_progress() if set:
        - MPI_Request_get_status 
        - MPI_Test (at least if it uses ompi_request_default_test)
    - mca_part_persist_component_init has enable_progress_threads parameter but does not use it


- opal_progress() is called by:
    - MPI_Parrived
    - MPI_Request_get_status
    - MPI_Wait
    - MPI_Test
- opal_progress() effectively calls event_base_loop() in libevent, but no extra thread used?
