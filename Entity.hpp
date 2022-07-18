#pragma once
#include "Vec2d.hpp"


#define COMBAT_TURN_SECONDS 2.0
#define MOVEMENT_TURN_SECONDS 0.25

constexpr double WELLFED_SECONDS_PER_REGEN = 3.0;


enum DIRECTION {
	DIRECTION_NONE = 0 ,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	// angled:
	DIRECTION_ANGLED_DOWN_RIGHT,
	DIRECTION_ANGLED_DOWN_LEFT,
	DIRECTION_ANGLED_UP_RIGHT,
	DIRECTION_ANGLED_UP_LEFT,
};




typedef struct Entity Entity;

struct Entity {
	Vec2d vec2d_position = Vec2d(1,1);
	Vec2d vec2d_position_last = Vec2d(1,1);
	double total_seconds= 0.0;
	CountdownTimer timer_combat_turn = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_movement = CountdownTimer(MOVEMENT_TURN_SECONDS);
	CountdownTimer timer_life = CountdownTimer(0);
	CountdownTimer timer_regenerate_life = CountdownTimer(WELLFED_SECONDS_PER_REGEN);
	CountdownTimer timer_wellfed = CountdownTimer(20.0);

	int ncurses_symbol = '@';
	int ncurses_attrs = 0;

	bool has_collision = true;
	bool is_timed_life = false;

	bool has_selected_target = false;
	size_t id_of_target = 0;
	void reset_targeting(void) { id_of_target = 0 ; has_selected_target = false; };

	// combat-things
	int stat_life_max = 8;
	int stat_life = 1;

//ctor constructor
	Entity()
	{
		timer_movement.remaining_seconds = 0.0;
		timer_wellfed.remaining_seconds     = 20.0;
	}

// timer
	void update_with_seconds(double const deltatime_seconds);


// general
	bool is_dead(void) const { return false; } // TODO

	bool is_blocking(void) const { 
		if(!has_collision) {
			return false;
		}
		if(is_dead()) {
			return false;
		}
		return true;
	}


	// ncurses
	void wprint_detailed_entity_info(WINDOW * w) const;


	// movement
private:
	enum DIRECTION direction = DIRECTION_NONE;
	enum DIRECTION direction_persistent = DIRECTION_NONE;

public:
	enum DIRECTION direction_persistent_ai = DIRECTION_NONE;
	void set_direction_persistent(enum DIRECTION const dir) { direction_persistent = dir; }
	void set_direction_temporary(enum DIRECTION const dir) { direction = dir; direction_persistent = DIRECTION_NONE; }

	void update_position(void) {
		if(direction_persistent != DIRECTION_NONE) {
			direction = direction_persistent;
		}
		vec2d_position_last = vec2d_position;
		// TODO only update if ready to move
		timer_movement.reset_countdown();
		switch(direction) {
			case DIRECTION_UP    : --(vec2d_position.y); break;
			case DIRECTION_DOWN  : ++(vec2d_position.y); break;
			case DIRECTION_LEFT  : --(vec2d_position.x); break;
			case DIRECTION_RIGHT : ++(vec2d_position.x); break;
			case DIRECTION_ANGLED_DOWN_RIGHT : ++(vec2d_position.y); ++(vec2d_position.x); timer_movement.remaining_seconds *= 2;  break;
			case DIRECTION_ANGLED_DOWN_LEFT  : ++(vec2d_position.y); --(vec2d_position.x); timer_movement.remaining_seconds *= 2;  break;
			case DIRECTION_ANGLED_UP_RIGHT   : --(vec2d_position.y); ++(vec2d_position.x); timer_movement.remaining_seconds *= 2;  break;
			case DIRECTION_ANGLED_UP_LEFT    : --(vec2d_position.y); --(vec2d_position.x); timer_movement.remaining_seconds *= 2;  break;
			case DIRECTION_NONE : break;
		}
		
		direction = direction_persistent;
	}

	void position_restore_last(void) {
		vec2d_position = vec2d_position_last;
		direction = DIRECTION_NONE;
		direction_persistent = DIRECTION_NONE;
	}


	void update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER);
};








	void
Entity::update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER)
{
	total_seconds += GLOBALTIMER.deltatime_seconds;

	timer_combat_turn.update_time_from_globaltimer(GLOBALTIMER);
	timer_movement.update_time_from_globaltimer(GLOBALTIMER);
	timer_life.update_time_from_globaltimer(GLOBALTIMER);


	// movement
	if(direction_persistent_ai != DIRECTION_NONE) {
		direction_persistent = direction_persistent_ai;
	}
	if(direction_persistent != DIRECTION_NONE) {
		direction = direction_persistent;
	}
	if(direction != DIRECTION_NONE) {
		int const tick_movement = timer_movement.consume_tick();
		if(tick_movement >= 1) {
			update_position();
		}
	}


	// wellfed/life regen
	timer_regenerate_life.update_time_from_globaltimer(GLOBALTIMER);
	timer_wellfed.update_time_from_globaltimer(GLOBALTIMER);
	if(timer_wellfed.remaining_seconds > 0) {
		if(timer_regenerate_life.consume_tick()) {
			++stat_life;
			timer_regenerate_life.reset_countdown();
		}
	}


}




	void
Entity::wprint_detailed_entity_info(WINDOW * w) const
{
	assert(w);
	
	werase(w);
	box(w,0,0);
	wmove(w,1,1);
	wprintw(w,"%c  %2d/%2d" , ncurses_symbol , stat_life , stat_life_max);
	wmove(w,2,1);
	wprintw(w,"%6.1f" , timer_regenerate_life.remaining_seconds);
	wmove(w,3,1);
	wprintw(w,"%6.1f" , timer_wellfed.remaining_seconds);
	wrefresh(w);
}




