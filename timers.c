#include "timers.h"

#include <stdlib.h>
#include <mpi.h>
#include <math.h>

struct timer {
	double sum;
	int count;
	double average;
	double standard_deviation;
	double running_since;
};

void timer_init(struct timer* t) {
	t->running_since = 0;

	// initialize statistics
	t->sum = 0;
	t->count = 0;
	t->average = 0;

	// not the actual standard deviation (can be derived by dividing by count)
	t->standard_deviation = 0;
}

void timer_start(struct timer* t) {
	t->running_since = MPI_Wtime();
}

void timer_stop(struct timer* t) {
	double delta = MPI_Wtime() - t->running_since;

	t->count += 1;
	t->sum += delta;

	double new_average = t->average + (delta / (double)t->count);

	// compute standard deviation (https://www.johndcook.com/blog/standard_deviation/)
	t->standard_deviation += (delta - t->average) * (delta - new_average);
	t->average = new_average;
}

double timer_mean(struct timer* t) {
	return t->average;
}

double timer_std_dev(struct timer* t) {
	return sqrt(t->standard_deviation / (double)(t->count - 1));
}

void timers_init(struct timer** timers, int num_timers)
{
    *timers = malloc(sizeof(struct timer) * num_timers);
    for (int i = 0; i < num_timers; i++) {
        timer_init(&(*timers)[i]);
    }
}

void timers_start(struct timer* timers, int t) {
    timer_start(&timers[t]);
}
struct timer* timers_get(struct timer* timers, int t) {
    return &timers[t];
}

void timers_stop(struct timer* timers, int t) {
    timer_stop(&timers[t]);
}

void timers_free(struct timer *timers)
{
    free(timers);
}