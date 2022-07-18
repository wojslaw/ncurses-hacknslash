#pragma once
#include "Vec2d.hpp"


#define COMBAT_TURN_SECONDS


enum DIRECTION {
	DIRECTION_NONE = 0 ,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
};




typedef struct Entity Entity;

struct Entity {
	Vec2d vec2d_position = Vec2d(0,0);
	Vec2d vec2d_position_last = Vec2d(0,0);
	CountdownTimer timer_combat_turn = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_movement = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_life = CountdownTimer(0);

	enum DIRECTION direction = DIRECTION_NONE;
	enum DIRECTION direction_persistent = DIRECTION_NONE;

	bool has_collision = true;
	bool is_timed_life = false;

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
	int ncurses_symbol = '@';
	int ncurses_attrs = 0;

	void update_position(void) {
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



	void update_with_seconds(double const deltatime_seconds);
};

