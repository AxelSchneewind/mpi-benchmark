#include "timer.h"
#include "mpi.h"
#include <math.h>

void timer_init(timer* t) {
	t->running_since = 0;

	// initialize statistics
	t->sum = 0;
	t->count = 0;
	t->average = 0;

	// not the actual standard deviation (can be derived by dividing by count)
	t->standard_deviation = 0;
}

void timer_start(timer* t) {
	t->running_since = MPI_Wtime();
}

void timer_stop(timer* t) {
	double delta = MPI_Wtime() - t->running_since;

	t->count += 1;
	t->sum += delta;

	double new_average = t->average + (delta / (double)t->count);

	// compute standard deviation (https://www.johndcook.com/blog/standard_deviation/)
	t->standard_deviation += (delta - t->average) * (delta - new_average);
	t->average = new_average;
}

double timer_mean(timer* t) {
	return t->average;
}

double timer_std_dev(timer* t) {
	return sqrt(t->standard_deviation / (double)(t->count - 1));
}
