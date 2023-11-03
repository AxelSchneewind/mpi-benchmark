#pragma once

typedef struct {
	double sum;
	double running_since;
} timer;

extern void timer_init(timer* t);

extern void timer_start(timer* t);
extern void timer_stop(timer* t);