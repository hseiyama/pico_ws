struct sys_time {
	uint64_t u64_time;
	bool bl_state;
};

extern void sys_timer_start(struct sys_time *);
extern void sys_timer_stop(struct sys_time *);
extern bool sys_timer_check(struct sys_time *, uint64_t);
extern uint64_t sys_timer_diff(struct sys_time *, uint64_t);
extern bool sys_timer_isrun(struct sys_time *);
