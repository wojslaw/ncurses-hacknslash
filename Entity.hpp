#pragma once
#include "Vec2d.hpp"


#define COMBAT_TURN_SECONDS 2.0
#define MOVEMENT_TURN_SECONDS 0.5


enum DIRECTION {
	DIRECTION_NONE = 0 ,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
};




typedef struct Entity Entity;

struct Entity {
	Vec2d vec2d_position = Vec2d(1,1);
	Vec2d vec2d_position_last = Vec2d(1,1);
	double total_seconds= 0.0;
	CountdownTimer timer_combat_turn = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_movement = CountdownTimer(MOVEMENT_TURN_SECONDS);
	CountdownTimer timer_life = CountdownTimer(0);

	int ncurses_symbol = '@';
	int ncurses_attrs = 0;

	bool has_collision = true;
	bool is_timed_life = false;

//ctor constructor
	Entity()
	{
		timer_movement.remaining_seconds = 0.0;
		timer_combat_turn.is_timer_repeating = true;
		timer_movement.is_timer_repeating = false;
		timer_life.is_timer_repeating     = false;
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


	// movement
private:
	enum DIRECTION direction = DIRECTION_NONE;
	enum DIRECTION direction_persistent = DIRECTION_NONE;

public:
	void set_direction_persistent(enum DIRECTION const dir) { direction_persistent = dir; }
	void set_direction_temporary(enum DIRECTION const dir) { direction = dir; direction_persistent = DIRECTION_NONE; }

	void update_position(void) {
		if(direction_persistent != DIRECTION_NONE) {
			direction = direction_persistent;
		}
		vec2d_position_last = vec2d_position;
		// TODO only update if ready to move
		switch(direction) {
			case DIRECTION_UP    : --(vec2d_position.y); break;
			case DIRECTION_DOWN  : ++(vec2d_position.y); break;
			case DIRECTION_LEFT  : --(vec2d_position.x); break;
			case DIRECTION_RIGHT : ++(vec2d_position.x); break;
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

	timer_combat_turn.update_from_globaltimer(GLOBALTIMER);
	timer_movement.update_from_globaltimer(GLOBALTIMER);
	timer_life.update_from_globaltimer(GLOBALTIMER);

	if(direction_persistent != DIRECTION_NONE) {
		direction = direction_persistent;
	}
	// movement
	if(direction != DIRECTION_NONE) {
		int const tick_movement = timer_movement.consume_tick();
		if(tick_movement >= 1) {
			update_position();
			timer_movement.reset_countdown();
		}
	}


}
