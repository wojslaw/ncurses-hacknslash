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

	double update_auto(void);

	// ctor
	GlobalTimer() ;
};













struct CountdownTimer {
	double seconds_countdown = 0.0;
	double total_seconds = 0.0;
	double remaining_seconds = 0.0;
	int ticks_collected = 0;
	bool collect_ticks = false; // TODO? for timers that shouldn't have to be resetted

	CountdownTimer();
	CountdownTimer(double const _remaining_seconds);

	void reset_countdown(void) ;

	void reset(void) ;

	bool is_countdown_finished(void) const ;

	void update_with_deltatime_seconds(double const deltatime_seconds) ;
	void update_time_from_globaltimer( GlobalTimer const & global_timer) ;

	int consume_tick(void) ;

	int consume_all_ticks(void) ;
};


