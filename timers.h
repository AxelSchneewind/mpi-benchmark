struct timer;
extern void timer_init(struct timer* t);

extern void timer_start(struct timer* t);
extern void timer_stop(struct timer* t);

extern double timer_mean(struct timer* t);
extern double timer_std_dev(struct timer* t);

typedef struct timer* timers;

extern void timers_start(timers timers, int t);
extern void timers_stop(timers timers, int t);
extern struct timer* timers_get(timers timers, int t);
extern void timers_init(timers* timers, int num_timers);
extern void timers_free(timers timers);
