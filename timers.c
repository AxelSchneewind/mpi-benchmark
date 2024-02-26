#include "timers.h"

#include <stdlib.h>

void timers_init(timer **timers)
{
    *timers = malloc(sizeof(timer) * TimerCount);
    for (int i = 0; i < TimerCount; i++) {
        timer_init(&(*timers)[i]);
    }
}

void timers_start(timer* timers, Timer t) {
    timer_start(&timers[t]);
}

void timers_stop(timer* timers, Timer t) {
    timer_stop(&timers[t]);
}

void timers_free(timer *timers)
{
    free(timers);
}