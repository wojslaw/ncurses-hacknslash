#include "Timer.hpp"




GlobalTimer::GlobalTimer()
{
	// int const errval = // TODO
	gettimeofday(&time_last,0);
	// TODO handle errorvalue
	time_new = time_last;
}


	double
GlobalTimer::update_with_multiplier(double const multiplier)
{
	//int const errval = // TODO
	gettimeofday(&time_new,0);
	new_deltatime_seconds
		=  ((double)time_new.tv_sec  - (double)time_last.tv_sec )
		+ (((double)time_new.tv_usec - (double)time_last.tv_usec) / 1000000) ;
	deltatime_seconds += new_deltatime_seconds * multiplier;
	time_last = time_new;
	total_seconds += deltatime_seconds;
	return deltatime_seconds;
}

	void 
GlobalTimer::reset(void)
{
	deltatime_seconds = 0;
}

















CountdownTimer::CountdownTimer()
	: seconds_countdown(0)
	, remaining_seconds(0)
{ }


CountdownTimer::CountdownTimer(
		double const _remaining_seconds
		)
	: seconds_countdown(_remaining_seconds)
	, remaining_seconds(_remaining_seconds)
{ }


CountdownTimer::CountdownTimer(
		 double const _remaining_seconds
		,double const _start)
	: seconds_countdown(_remaining_seconds)
	, remaining_seconds(_start)
{ }


	void
CountdownTimer::reset_countdown_keep_ticks(void)
{
	remaining_seconds = seconds_countdown;
}

	void 
CountdownTimer::reset(void)
{
	remaining_seconds = seconds_countdown;
	ticks_collected = 0;
}




	bool 
CountdownTimer::is_countdown_finished(void) const
{
	return ticks_collected > 0;
}




	void
CountdownTimer::update_with_deltatime_seconds(double const deltatime_seconds)
{
	total_seconds += deltatime_seconds;
	remaining_seconds -= deltatime_seconds;
	if(remaining_seconds < 0) {
		if(collect_ticks) {
			remaining_seconds += seconds_countdown;
			++ticks_collected;
		} else {
			ticks_collected = 1;
			remaining_seconds = 0;
		}
	}
}




	void 
CountdownTimer::update_time_from_globaltimer( GlobalTimer const & global_timer)
{
	update_with_deltatime_seconds(global_timer.deltatime_seconds);
}




	int 
CountdownTimer::consume_tick(void)
{
	if(ticks_collected < 1) {
		return 0;
	}
	int const ticks = ticks_collected;
	ticks_collected = 0;
	return ticks;
}




	int
CountdownTimer::consume_all_ticks(void)
{
	int const ticks = ticks_collected;
	ticks_collected = 0;
	return ticks;
}













