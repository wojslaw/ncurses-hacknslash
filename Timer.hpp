#pragma once
#include <time.h>
#include <sys/time.h>




typedef struct CountdownTimer CountdownTimer;
typedef struct GlobalTimer GlobalTimer;








struct GlobalTimer {
	double total_seconds = 0.0;
	double deltatime_seconds = 0.0;
	struct timeval time_last = {0,0};
	struct timeval time_new = {0,0};

	double update_auto(void) {
		//int const errval = // TODO
		gettimeofday(&time_new,0);
		deltatime_seconds
			=  ((double)time_new.tv_sec  - (double)time_last.tv_sec )
			+ (((double)time_new.tv_usec - (double)time_last.tv_usec) / 1000000) ;
		time_last = time_new;
		total_seconds += deltatime_seconds;
		return deltatime_seconds;
	}

	GlobalTimer() {
		// int const errval = // TODO
		gettimeofday(&time_last,0);
		// TODO handle errorvalue
		time_new = time_last;
	}
};













struct CountdownTimer {
	double seconds_countdown = 0.0;
	double total_seconds = 0.0;
	double remaining_seconds = 0.0;
	int ticks_collected = 0;
	bool is_timer_repeating = true;

	CountdownTimer() { }
	CountdownTimer(double const _remaining_seconds) { 
		seconds_countdown = _remaining_seconds;
		remaining_seconds = _remaining_seconds;
	}

	void reset_countdown(void) {
		remaining_seconds = seconds_countdown;
	}

	bool is_countdown_finished(void) const { return ticks_collected > 0; }

	void update_with_deltatime_seconds(double const deltatime_seconds) {
		total_seconds += deltatime_seconds;
		if(seconds_countdown > 0) {
			remaining_seconds -= deltatime_seconds;
			if(remaining_seconds < 0) {
				++ticks_collected;
				if(is_timer_repeating) {
					remaining_seconds += seconds_countdown;// TODO consume all into ticks
				}
			}
		}
	}

	void update_time_from_globaltimer( GlobalTimer const & global_timer) {
		update_with_deltatime_seconds(global_timer.deltatime_seconds);
	}

	int consume_tick(void) {
		if(ticks_collected < 1) {
			return 0;
		}
		int const ticks = ticks_collected;
		ticks_collected = 0;
		return ticks;
	}

	int consume_all_ticks(void) {
		int const ticks = ticks_collected;
		ticks_collected = 0;
		return ticks;
	}
};


