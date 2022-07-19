#pragma once
#include "Vec2d.hpp"
#include "Timer.hpp"
#include "Ncurses.hpp"


#define COMBAT_TURN_SECONDS 2.0
#define MOVEMENT_TURN_SECONDS 0.25
#define SECONDS_CORPSE_DECAY 8.0

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
	// 
	DIRECTION_COUNT
};


const Vec2d DIRECTION_VECTOR[DIRECTION_COUNT] = {
	[DIRECTION_NONE] = Vec2d( 0, 0) ,
	[DIRECTION_UP     ] = Vec2d(-1, 0) , 
	[DIRECTION_DOWN   ] = Vec2d(+1, 0) , 
	[DIRECTION_LEFT   ] = Vec2d( 0,-1) , 
	[DIRECTION_RIGHT  ] = Vec2d( 0, 1) , 
	[DIRECTION_ANGLED_DOWN_RIGHT] = Vec2d( 1, 1) ,
	[DIRECTION_ANGLED_DOWN_LEFT ] = Vec2d( 1,-1) ,
	[DIRECTION_ANGLED_UP_RIGHT  ] = Vec2d(-1, 1) ,
	[DIRECTION_ANGLED_UP_LEFT   ] = Vec2d(-1,-1) ,
};





typedef struct Entity Entity;

struct Entity {
	Vec2d vec2d_position = Vec2d(1,1);
	Vec2d vec2d_position_last = Vec2d(1,1);
	Vec2d vec2d_planned_movement = Vec2d(0,0);
	Vec2d vec2d_last_movement = Vec2d(0,0);
	bool was_moved_recently(void) const {
		return
			(vec2d_last_movement.y != 0)
			||
			(vec2d_last_movement.x != 0)
			;
	}
	bool flag_moved_last_turn = false;

	//
	double total_seconds= 0.0;
	CountdownTimer timer_combat_turn = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_movement = CountdownTimer(MOVEMENT_TURN_SECONDS);
	CountdownTimer timer_life = CountdownTimer(0);
	CountdownTimer timer_regenerate_life = CountdownTimer(WELLFED_SECONDS_PER_REGEN);
	CountdownTimer timer_wellfed = CountdownTimer(20.0);
	CountdownTimer timer_recently_hit = CountdownTimer(0.5);
	CountdownTimer timer_decay = CountdownTimer(SECONDS_CORPSE_DECAY);

	//
	bool flag_skip_update = false;
	bool flag_has_collision = true;
	bool is_timed_life = false;
	bool flag_follow_target = true;
	bool flag_stop_on_collision = true;




	int ncurses_symbol = '@';
	int ncurses_get_attrs(void) const {
		if(timer_recently_hit.remaining_seconds > 0.0) {
			return ATTR_RECENTLY_HIT;
		}
		return 0;
	}


	bool has_selected_target = false;
	size_t id_of_target = 0;
	void reset_targeting(void) {
		id_of_target = 0 ;
		has_selected_target = false;
	}
	void set_target_to_entityid(size_t const entityid) { 
		id_of_target = entityid ; 
		has_selected_target = true;
	}

	bool is_ready_to_attack(void) const {
		if(is_dead()) {
			return false;
		}
		return timer_combat_turn.remaining_seconds <= 0;
	}

	// combat-things
	unsigned randomness_seed_combat = 0;

	int stat_life_max = 20;
	int stat_life = 12;
	void set_life_to_max(void) { stat_life = stat_life_max; }


	int get_defense() const { return 1; }
	int get_attack_base() const    { return 1; }
	int get_attack_dice() const    { return 2; }
	int get_attack_maximum() const { return get_attack_base()+(1+get_attack_dice()); }

	int last_combat_attack_roll = -1;
	int last_combat_attack_damage = -1;

	bool is_ready_to_move(void) const {
		return (timer_movement.remaining_seconds <= 0.0);
	}


	bool is_alive(void) const {
		return stat_life >= 0;
	}
	bool is_dead(void) const {
		return stat_life < 0;
	}
	bool is_fully_decayed(void) const {
		return is_dead() && timer_decay.remaining_seconds <= 0;
	}
	bool is_corpse(void) const {
		return is_dead() && timer_decay.remaining_seconds > 0;
	}
	int ncurses_get_symbol(void) const {
		if(is_dead()) {
			if(timer_decay.remaining_seconds > 0) {
				return '%';
			}
			return 0;
		}
		return ncurses_symbol;
	}


	int combat_get_range(void) const {
		if(ncurses_symbol == '@') {
			return 6;
		}
		if(ncurses_symbol == 'g') {
			return 8;
		}
		if(ncurses_symbol == 'w') {
			return 1;
		}
		return 2;
	} // just a random given number for now
	int combat_roll_damage(void) {
		if(is_dead()) {
			return 0;
		}
		if(timer_combat_turn.remaining_seconds > 0.0) {
			return 0;
		}
		timer_combat_turn.reset();
		last_combat_attack_roll = rand_r(&randomness_seed_combat) % (1+get_attack_dice());
		int const damage_rolled = last_combat_attack_roll + get_attack_base();
		if(damage_rolled > 0) {
			return damage_rolled;
		}
		return 0;
	}

	int combat_roll_damage_against_defense(int const defense) {
		last_combat_attack_damage = combat_roll_damage() - defense;
		return last_combat_attack_damage;
	}

	void take_damage(int const damage_to_take) {
		assert(damage_to_take >= 0);
		if(damage_to_take <= 0) {
			return;
		}
		stat_life -= damage_to_take;
		timer_recently_hit.reset();
	}
	void regen_life(int const delta_life) {
		if(is_dead()) {
			return;
		}
		stat_life += delta_life;
		if(stat_life > stat_life_max) {
			stat_life = stat_life_max;
		}
	}

//ctor constructor
	Entity()
	{
		timer_movement.remaining_seconds = 0.0;
		timer_wellfed.remaining_seconds     = 20.0;
	}

// timer
	void update_with_seconds(double const deltatime_seconds);


// general

	bool is_blocking(void) const { 
		if(!flag_has_collision) {
			return false;
		}
		if(is_dead()) {
			if(timer_decay.remaining_seconds <= 0) {
				return false;
			}
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
	void set_direction_persistent(enum DIRECTION const dir);
	void set_direction_order(enum DIRECTION const dir) ;

	void order_stop(void);

	void update_movement_from_planned_movement(void);

	void update_movement(void);

	void position_restore_last(void) ;

	void update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER);

	bool has_planned_movement(void);

};





