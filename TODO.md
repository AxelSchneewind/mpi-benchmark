- ucx: for <= 1KB message, network card progress

- use CPE -> cray-mpich-ucx does not implement partitioned communication


- openmpi ucx gegen ob1 tauschen
    - mpirun --mca pml ob1
        - on hawk: seems to get stuck at Isend with 2 partitions
    - mpirun --mca pml ucx


# benchmarks:

## OpenMPI
-   psend_parrived: Parrived() after MPI_Request_get_status(), sometimes completion flag == true and Parrived flag == false
    - sometimes Parrived doesnt return flag==true for all partitions
    - see parrived.c
-   psend_progress: MPI_Request_get_status() returns true before all Pready calls are done, also sometimes gets stuck in MPI_Wait() or MPI_Request_get_status()
    - see get_request.c

## MPICH
- Psend has low bandwidth for all partition sizes


- enable openmpi progress threads?
    - OPAL_ENABLE_PROGRESS_THREADS defined (=0) by default
    - functions call opal_progress() if set:
        - MPI_Request_get_status 
        - MPI_Test (at least if it uses ompi_request_default_test)



- opal_progress() is called by:
    - MPI_Parrived
    - MPI_Request_get_status
    - MPI_Wait
    - MPI_Test
- opal_progress() effectively calls event_base_loop() in libevent, but no extra thread used


- progress triggern mit MPI_Request_Get_Status()