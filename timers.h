#include "timer.h"

typedef enum {
    Total = 0,
    Iteration,
    IterationStartToWait,
    IterationWait,
    TimerCount
} Timer;

extern void timers_start(timer* timers, Timer t);
extern void timers_stop(timer* timers, Timer t);
extern void timers_init(timer** timers);
extern void timers_free(timer* timers);
