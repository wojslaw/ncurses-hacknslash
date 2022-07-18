#pragma once
#include "Vec2d.hpp"


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

bool is_direction_angled(enum DIRECTION direction)
{
	return
		direction == DIRECTION_ANGLED_DOWN_RIGHT
		||
		direction == DIRECTION_ANGLED_DOWN_LEFT 
		||
		direction == DIRECTION_ANGLED_UP_RIGHT  
		||
		direction == DIRECTION_ANGLED_UP_LEFT   
		;
}




typedef struct Entity Entity;

struct Entity {
	Vec2d vec2d_position = Vec2d(1,1);
	Vec2d vec2d_position_last = Vec2d(1,1);
	Vec2d vec2d_direction_planned = Vec2d(0,0);
	double total_seconds= 0.0;
	CountdownTimer timer_combat_turn = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_movement = CountdownTimer(MOVEMENT_TURN_SECONDS);
	CountdownTimer timer_life = CountdownTimer(0);
	CountdownTimer timer_regenerate_life = CountdownTimer(WELLFED_SECONDS_PER_REGEN);
	CountdownTimer timer_wellfed = CountdownTimer(20.0);
	CountdownTimer timer_recently_hit = CountdownTimer(0.5);
	CountdownTimer timer_decay = CountdownTimer(SECONDS_CORPSE_DECAY);


	bool flag_skip_update = false;
	bool has_collision = true;
	bool is_timed_life = false;
	bool flag_follow_target = true;




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

	int stat_life_max = 8;
	int stat_life = 1;


	int get_defense() const { return 1; }
	int get_attack_base() const    { return 1; }
	int get_attack_dice() const    { return 2; }
	int get_attack_maximum() const { return get_attack_base()+(1+get_attack_dice()); }

	int last_combat_attack_roll = -1;
	int last_combat_attack_damage = -1;


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
		if(!has_collision) {
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
	void set_direction_persistent(enum DIRECTION const dir) { direction_persistent = dir; }
	void set_direction_temporary(enum DIRECTION const dir) ;


	void update_movement(void) {
		if(is_dead()) {
			return;
		}
		if(direction_persistent != DIRECTION_NONE) {
			direction = direction_persistent;
		}
		vec2d_position_last = vec2d_position;
		// TODO only update if ready to move
		timer_movement.reset_countdown();
		// TODO use vec2d_direction_planned
		//
		Vec2d const & direction_vector = DIRECTION_VECTOR[direction];
		vec2d_position.add_vec2d(direction_vector);
		if(is_direction_angled(direction)) {
			timer_movement.remaining_seconds *= 2;
		}
		direction = direction_persistent;
	}

	void position_restore_last(void) {
		vec2d_position = vec2d_position_last;
		direction = DIRECTION_NONE;
		direction_persistent = DIRECTION_NONE;
		vec2d_direction_planned = DIRECTION_VECTOR[DIRECTION_NONE];
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
	timer_recently_hit.update_time_from_globaltimer(GLOBALTIMER);

	if(is_dead()) {
		timer_decay.update_time_from_globaltimer(GLOBALTIMER);
	}

	vec2d_position_last = vec2d_position;
	// movement
	if(direction_persistent_ai != DIRECTION_NONE) {
		direction_persistent = direction_persistent_ai;
	}
	if(direction_persistent != DIRECTION_NONE) {
		direction = direction_persistent;
		vec2d_direction_planned = DIRECTION_VECTOR[direction_persistent];
	}
	if(direction != DIRECTION_NONE) {
		int const tick_movement = timer_movement.consume_tick();
		if(tick_movement >= 1) {
			update_movement();
		}
	}


	// wellfed/life regen
	timer_regenerate_life.update_time_from_globaltimer(GLOBALTIMER);
	timer_wellfed.update_time_from_globaltimer(GLOBALTIMER);
	if(timer_wellfed.remaining_seconds > 0) {
		if(timer_regenerate_life.consume_tick()) {
			regen_life(1);
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
	wmove(w,4,1);
	wprintw(w,"A:%d-%d , last: %d (rolled %d)"
			,get_attack_base()
			,get_attack_maximum()
			,last_combat_attack_damage
			,last_combat_attack_roll );
	wmove(w,5,1);
	wprintw(w,"[%2d , %2d]"
			,vec2d_direction_planned.y
			,vec2d_direction_planned.x );
	wrefresh(w);
}











	void
Entity::set_direction_temporary(enum DIRECTION const dir) 
{
	Vec2d const & vec2d_direction = DIRECTION_VECTOR[dir];
	vec2d_direction_planned.add_vec2d(vec2d_direction);
	vec2d_direction_planned.normalize();
	// set angled
	if(direction == DIRECTION_NONE) {
		direction = dir;
		return;
	}
	if(direction == dir) {
		direction_persistent = dir;
		return;
	}
	direction_persistent = DIRECTION_NONE;
	// combine angled direction
	if( direction == DIRECTION_UP ) {
		if(dir == DIRECTION_LEFT) {
			direction = DIRECTION_ANGLED_UP_LEFT;
			return;
		}
		if(dir == DIRECTION_RIGHT) {
			direction = DIRECTION_ANGLED_UP_RIGHT;
			return;
		}
	}
	if( direction == DIRECTION_DOWN ) {
		if(dir == DIRECTION_LEFT) {
			direction = DIRECTION_ANGLED_DOWN_LEFT;
			return;
		}
		if(dir == DIRECTION_RIGHT) {
			direction = DIRECTION_ANGLED_DOWN_RIGHT;
			return;
		}
	}
	if( direction == DIRECTION_LEFT ) {
		if(dir == DIRECTION_UP) {
			direction = DIRECTION_ANGLED_UP_LEFT;
			return;
		}
		if(dir == DIRECTION_DOWN) {
			direction = DIRECTION_ANGLED_DOWN_LEFT;
			return;
		}
	}
	if( direction == DIRECTION_LEFT ) {
		if(dir == DIRECTION_UP) {
			direction = DIRECTION_ANGLED_UP_LEFT;
			return;
		}
		if(dir == DIRECTION_DOWN) {
			direction = DIRECTION_ANGLED_DOWN_LEFT;
			return;
		}
	}
	if( direction == DIRECTION_RIGHT ) {
		if(dir == DIRECTION_UP) {
			direction = DIRECTION_ANGLED_UP_RIGHT;
			return;
		}
		if(dir == DIRECTION_DOWN) {
			direction = DIRECTION_ANGLED_DOWN_RIGHT;
			return;
		}
	}
	// or just set
	direction = dir;
}
