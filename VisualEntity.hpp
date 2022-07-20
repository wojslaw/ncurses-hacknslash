#pragma once
#include "Vec2d.hpp"
#include "Timer.hpp"


typedef struct VisualEntity VisualEntity;

struct VisualEntity {
	Vec2d vec2d_position = Vec2d(1,1);
	double total_seconds = 0.0;
	CountdownTimer timer_life = CountdownTimer(0.40);
	CountdownTimer timer_bold = CountdownTimer(0.25);
	int ncurses_symbol = 'x';
	// bool flag_is_on_top = true; // I'd have to loop the visual effects twice
	int ncurses_attrs = ATTR_RECENTLY_HIT;
	bool is_valid(void) const { return timer_life.remaining_seconds > 0.0; }


	// ctors
	VisualEntity(
			 int const _y
			,int const _x
			)
		: vec2d_position(Vec2d(_y,_x))
	{ }
	VisualEntity(
			 Vec2d const _vec2d_position
			)
		: vec2d_position(_vec2d_position)
	{ }

	VisualEntity(
			 int const _ncurses_symbol
			,double const time_of_expiration
			,int const _y
			,int const _x
			)
	{
		timer_life = CountdownTimer(time_of_expiration);
		ncurses_symbol = _ncurses_symbol;
		vec2d_position.y = _y;
		vec2d_position.x = _x;
	}

	VisualEntity(
			 int const _ncurses_symbol
			,double const time_of_expiration
			,Vec2d const _vec2d_position
			)
	{
		timer_life = CountdownTimer(time_of_expiration);
		ncurses_symbol = _ncurses_symbol;
		vec2d_position = _vec2d_position;
	}

	VisualEntity(
			 int const _ncurses_symbol
			,double const time_of_expiration
			,int const _ncurses_attrs
			,int const _y
			,int const _x
			)
	{
		timer_life = CountdownTimer(time_of_expiration);
		ncurses_symbol = _ncurses_symbol;
		ncurses_attrs = _ncurses_attrs;
		vec2d_position.y = _y;
		vec2d_position.x = _x;
	}

	VisualEntity(
			 int const _ncurses_symbol
			,double const time_of_expiration
			,int const _ncurses_attrs
			,Vec2d const _vec2d_position
			)
	{
		timer_life = CountdownTimer(time_of_expiration);
		ncurses_symbol = _ncurses_symbol;
		ncurses_attrs  = _ncurses_attrs ;
		vec2d_position = _vec2d_position;
	}


	// end: ctor

	void update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER)
	{
		total_seconds += GLOBALTIMER.deltatime_seconds;
		timer_life.update_time_from_globaltimer(GLOBALTIMER);
		timer_bold.update_time_from_globaltimer(GLOBALTIMER);
	}

	int wprint(WINDOW * w) const {
		assert(w);
		if(timer_life.is_countdown_finished()) {
			return ERR;
		}
		int const attrs 
			= (timer_bold.is_countdown_finished())
			? ncurses_attrs
			: ncurses_attrs | WA_BOLD ;
		if(attrs) {
			wattron(w, attrs);
		}
		waddch(w,ncurses_symbol);
		if(attrs) {
			wattroff(w, attrs);
		}
		return OK;
	}
	// bool flag_is_on_top = true; // I'd have to loop the visual effects twice
};








