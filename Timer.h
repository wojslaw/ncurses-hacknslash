#pragma once
#include <time.h>




typedef struct CountdownTimer CountdownTimer;
typedef struct GlobalTimer GlobalTimer;


struct CountdownTimer {
	double seconds_countdown = 0.0;
	double total_seconds = 0.0;
	double remaining_seconds = 0.0;
	int ticks_collected = 0;


	bool is_countdown_finished(void) const { return ticks_collected > 0; }

	void update(double const deltatime_seconds) {
		total_seconds += deltatime_seconds;
		remaining_seconds -= deltatime_seconds;
		if(seconds_countdown > 0) {
			if(remaining_seconds < 0) {
				++ticks_collected;
				remaining_seconds += seconds_countdown;// TODO consume all
			}
		}
	}

	int consume_ticks(void) {
		int const ticks = ticks_collected;
		ticks_collected = 0;
		return ticks;
	};
};





struct GlobalTimer {
	double total_seconds = 0.0;


	struct timeval time_last = {0,0};
	struct timeval time_new = {0,0};
	double deltatime_seconds = 0.0;

	double update_from_global_time(void) {
		//int const errval = // TODO
		gettimeofday(&time_new,0);
		deltatime_seconds
			= (time_new.tv_sec  - time_last.tv_sec )
			+ ((time_new.tv_usec - time_last.tv_usec) * 1000 * 1000) ;
		return deltatime_seconds;
	}

	GlobalTimer() {
		// int const errval = // TODO
		gettimeofday(&time_last,0);
		// TODO handle errorvalue
		time_new = time_last;
	}
};
