#include "bench.h"
#include <mpi.h>
#include <memory.h>

// powers of two for buffer sizes
#define B (MPI_Count)1
#define KB (MPI_Count)(1024)
#define MB (MPI_Count)(1024 * 1024)
#define GB (MPI_Count)(1024 * 1024 * 1024)

struct setup_t {
    MPI_Count buffer_size;
    int iterations;
    int num_send_patterns;
    bool enable_mode[ModeCount];
    MPI_Count min_partition_size[ModeCount];
    MPI_Count max_partition_size[ModeCount];                                                                                         
    SendPattern send_patterns[SendPatternCount]; 
};

static const char* setup_names[] = {
    "OPENMPI_HAWK",
    "OPENMPI_LOCAL",
    "FULL_HAWK",
    "FULL_LOCAL",
    "RDMA_LOCAL",
    "PARTITIONED_LOCAL",
    "CUSTOM_LOCAL"
};

struct setup_t setups[] = {
    // openmpi
    {
        .buffer_size = 8 * MB,
        .iterations = 100,
        //                          Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 3, IsendThenTest = 4, IsendTestall = 5, CustomPsend = 6, WinSingle = 7,            Win = 8,   Psend = 9, PsendList = 10, PsendParrived = 11, PsendProgress = 12, PsendProgressThreaded = 13, PsendThreaded = 14
        .enable_mode =        {         true,         true,      true,          true,              true,             true,           false,          true,               true,        true,          true,              true,              false,                      false,               true},
        .min_partition_size = {          512,          512,       512,           512,               512,              512,             512,          2048,       8 * MB / 128,         512,           512,               512,                512,                        512,                512},
        .max_partition_size = {       8 * MB,       8 * MB,    8 * MB,        8 * MB,            8 * MB,           8 * MB,          8 * MB,        8 * MB,             8 * MB,      8 * MB,        8 * MB,            8 * MB,             8 * MB,                     8 * MB,             8 * MB},
        .send_patterns =      { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // openmpi local
    {
        .buffer_size = 8 * MB,
        .iterations = 10,
        //                          Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 3, IsendThenTest = 4, IsendTestall = 5, CustomPsend = 6, WinSingle = 7,            Win = 8,   Psend = 9, PsendList = 10, PsendParrived = 11, PsendProgress = 12, PsendProgressThreaded = 13, PsendThreaded = 14
        .enable_mode =        {         true,               true,      true,          true,              true,             true,           false,          true,               true,        true,          true,              true,              false,                      false,               true},
        .min_partition_size = {          512,                512,      2048,          2048,              2048,             2048,            2048,          2048,      8 * MB / 1024,        2048,          2048,              2048,               2048,                       2048,               2048},
        .max_partition_size = {       8 * MB,             8 * MB,    8 * MB,        8 * MB,            8 * MB,           8 * MB,          8 * MB,        8 * MB,             8 * MB,      8 * MB,        8 * MB,            8 * MB,             8 * MB,                     8 * MB,             8 * MB},
        .send_patterns =      { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // full bench on hawk
    {
        .buffer_size = 8 * MB,
        .iterations = 100,
        //                          Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 3, IsendThenTest = 4, IsendTestall = 5, CustomPsend = 6, WinSingle = 7,            Win = 8,   Psend = 9, PsendList = 10, PsendParrived = 11, PsendProgress = 12, PsendProgressThreaded = 13, PsendThreaded = 14
        .enable_mode =        {         true,               true,      true,          true,              true,             true,           false,          true,               true,        true,          true,              true,               true,                       true,               true},
        .min_partition_size = {          512,                512,       512,           512,               512,              512,             512,          2048,      8 * MB / 1024,         512,          2048,               512,                512,                        512,                512},
        .max_partition_size = {       8 * MB,             8 * MB,    8 * MB,        8 * MB,            8 * MB,           8 * MB,          8 * MB,        8 * MB,             8 * MB,      8 * MB,        8 * MB,            8 * MB,             8 * MB,                     8 * MB,             8 * MB},
        .send_patterns =      { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // full bench locally
    {
        .buffer_size = 8 * MB,
        .iterations = 30,
        //                          Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 3, IsendThenTest = 4, IsendTestall = 5, CustomPsend = 6, WinSingle = 7,            Win = 8,   Psend = 9, PsendList = 10, PsendParrived = 11, PsendProgress = 12, PsendProgressThreaded = 13, PsendThreaded = 14
        .enable_mode =        {         true,               true,      true,          true,              true,             true,           false,          true,               true,        true,          true,              true,               true,                       true,               true},
        .min_partition_size = {          512,                512,      2048,          2048,              2048,             2048,            2048,          2048,      8 * MB / 1024,        2048,          2048,              2048,               2048,                       2048,               2048},
        .max_partition_size = {       8 * MB,             8 * MB,    8 * MB,        8 * MB,            8 * MB,           8 * MB,          8 * MB,        8 * MB,             8 * MB,      8 * MB,        8 * MB,            8 * MB,             8 * MB,                     8 * MB,             8 * MB},
        .send_patterns =      { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // rdma local
    {
        .buffer_size = 8 * MB,
        .iterations = 10,
        //                          Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 3, IsendThenTest = 4, IsendTestall = 5, CustomPsend = 6, WinSingle = 7,            Win = 8,   Psend = 9, PsendList = 10, PsendParrived = 11, PsendProgress = 12, PsendProgressThreaded = 13, PsendThreaded = 14
        .enable_mode =        {         true,              false,     false,         false,             false,            false,           false,          true,               true,       false,         false,             false,              false,                      false,              false},
        .min_partition_size = {          512,                512,      2048,          2048,              2048,             2048,            2048,          2048,      8 * MB / 1024,        2048,          2048,              2048,               2048,                       2048,               2048},
        .max_partition_size = {       8 * MB,             8 * MB,    8 * MB,        8 * MB,            8 * MB,           8 * MB,          8 * MB,        8 * MB,             8 * MB,      8 * MB,        8 * MB,            8 * MB,             8 * MB,                     8 * MB,             8 * MB},
        .send_patterns =      { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // partitioned local
    {
        .buffer_size = 8 * MB,
        .iterations = 10,
        //                          Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 3, IsendThenTest = 4, IsendTestall = 5, CustomPsend = 6, WinSingle = 7,            Win = 8,   Psend = 9, PsendList = 10, PsendParrived = 11, PsendProgress = 12, PsendProgressThreaded = 13, PsendThreaded = 14
        .enable_mode =        {         true,              false,     false,         false,             false,            false,           false,         false,              false,        true,          true,              true,               true,                       true,               true},
        .min_partition_size = {          512,                512,      2048,          2048,              2048,             2048,            2048,          2048,      8 * MB / 1024,        2048,          2048,              2048,               2048,                       2048,               2048},
        .max_partition_size = {       8 * MB,             8 * MB,    8 * MB,        8 * MB,            8 * MB,           8 * MB,          8 * MB,        8 * MB,             8 * MB,      8 * MB,        8 * MB,            8 * MB,             8 * MB,                     8 * MB,             8 * MB},
        .send_patterns =      { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // custom local
    {
        .buffer_size = 8 * MB,
        .iterations = 10,
        //                          Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 3, IsendThenTest = 4, IsendTestall = 5, CustomPsend = 6, WinSingle = 7,            Win = 8,   Psend = 9, PsendList = 10, PsendParrived = 11, PsendProgress = 12, PsendProgressThreaded = 13, PsendThreaded = 14
        .enable_mode =        {         true,               true,     false,         false,             false,            false,            true,         false,              false,       false,          true,             false,              false,                      false,              false},
        .min_partition_size = {          512,                512,      2048,          2048,              2048,             2048,            2048,          2048,      8 * MB / 1024,        2048,          2048,              2048,               2048,                       2048,               2048},
        .max_partition_size = {       8 * MB,             8 * MB,    8 * MB,        8 * MB,            8 * MB,           8 * MB,          8 * MB,        8 * MB,             8 * MB,      8 * MB,        8 * MB,            8 * MB,             8 * MB,                     8 * MB,             8 * MB},
        .send_patterns =      { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    }
};

typedef struct setup_t* setup;


setup select_setup(const char* name) {
    for (int i = 0; i < sizeof(setups)/ sizeof(setups[0]); i++)
    {
        if (0 == strcasecmp(name, setup_names[i])) {
            return &setups[i];
        }
    }
    return NULL;
}
