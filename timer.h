#pragma once

typedef struct {
	double sum;
	int count;
	double average;
	double standard_deviation;
	double running_since;
} timer;

extern void timer_init(timer* t);

extern void timer_start(timer* t);
extern void timer_stop(timer* t);

extern double timer_mean(timer* t);
extern double timer_std_dev(timer* t);
