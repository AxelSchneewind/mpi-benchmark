#include "timer.h"
#include "mpi.h"

void timer_init(timer* t) {
	t->sum = 0;
	t->running_since = 0;
}

void timer_start(timer* t) {
	t->running_since = MPI_Wtime();
}
void timer_stop(timer* t) {
	t->sum += MPI_Wtime() - t->running_since;
}
