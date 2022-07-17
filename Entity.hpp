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
	CountdownTimer timer_combat_turn = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_movement = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_life = CountdownTimer(0);

	enum DIRECTION direction = DIRECTION_NONE;

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
		// only update if ready to move
		// TODO in level: check for collision/out-of-bounds
		switch(direction) {
			case DIRECTION_UP    : --(vec2d_position.y); return;
			case DIRECTION_DOWN  : ++(vec2d_position.y); return;
			case DIRECTION_LEFT  : --(vec2d_position.x); return;
			case DIRECTION_RIGHT : ++(vec2d_position.x); return;
			case DIRECTION_NONE : return;
		}
	}



	void update_with_seconds(double const deltatime_seconds);
};

