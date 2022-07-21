#pragma once
#include <string>
#include "Vec2d.hpp"
#include "Timer.hpp"
#include "Ncurses.hpp"
#include "BaseEntity.hpp"
#include "Ability.hpp"

#define COMBAT_TURN_SECONDS 2.0
#define MOVEMENT_TURN_SECONDS 0.25
#define SECONDS_CORPSE_DECAY 8.0

constexpr double REGEN_SECONDS_WELLFED =  3.0;
constexpr double REGEN_SECONDS_NORMAL  = 12.0;


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
	// ctor
	Entity(ID_BaseEntity const _id_of_base_entity);

	// base entity
	ID_BaseEntity id_base_entity = ID_BaseEntity_none;
	BaseEntity const& ref_base_entity(void) const { return baseentity_ref_from_id(id_base_entity); }

	// resources and persistent state
	int resource_food = 0;
	int resource_money = 0;
	int explevel_level = 0;
	int explevel_points = 0;

	int explevel_points_for_next_level(void) const {
		return 10+(explevel_level*2);
	}

	// combat-things stats
	int stat_life = 12;

	std::vector<Ability> vector_of_abilities = std::vector<Ability>();

	// position/movement
	Vec2d vec2d_position = Vec2d(1,1);
	Vec2d vec2d_position_last = Vec2d(1,1);
	Vec2d vec2d_planned_movement = Vec2d(0,0);
	Vec2d vec2d_last_movement = Vec2d(0,0);
	bool is_recently_moved(void) const {
		return
			(vec2d_last_movement.y != 0)
			||
			(vec2d_last_movement.x != 0)
			;
	}
	bool flag_moved_last_turn = false;

	void force_set_position_yx(int const y , int const x) {
		vec2d_position.y = y;
		vec2d_position.x = x;
		vec2d_position_last.y = y;
		vec2d_position_last.x = x;
		vec2d_planned_movement.y = 0;
		vec2d_planned_movement.x = 0;
		vec2d_last_movement.y = 0;
		vec2d_last_movement.x = 0;
	}



	// misc combat-things stats
	unsigned randomness_seed_combat = 0;
	bool has_selected_target = false;
	size_t id_of_target = 0;


	// time
	double total_seconds= 0.0;

	CountdownTimer timer_combat_turn = CountdownTimer(COMBAT_TURN_SECONDS);
	CountdownTimer timer_movement = CountdownTimer(MOVEMENT_TURN_SECONDS,0);
	CountdownTimer timer_life = CountdownTimer(0);
	CountdownTimer timer_regenerate_life = CountdownTimer(REGEN_SECONDS_NORMAL);
	CountdownTimer timer_wellfed = CountdownTimer(0,0);
	CountdownTimer timer_recently_hit = CountdownTimer(0.75,0);
	CountdownTimer timer_recently_hit_heavily = CountdownTimer(0.75,0);
	CountdownTimer timer_is_in_battle = CountdownTimer(8.0,0);
	CountdownTimer timer_decay = CountdownTimer(SECONDS_CORPSE_DECAY);
	CountdownTimer timer_recently_healed = CountdownTimer(0.75,0);;
	CountdownTimer timer_ability  = CountdownTimer(1.0,0);;

	CountdownTimer timer_last_message = CountdownTimer(2.0,0);
	std::string last_message;

	//
	bool flag_skip_update = false;
	bool flag_marked_for_deletion = false;
	bool has_collision(void) const { return ref_base_entity().flag_has_collision; }
	bool has_destroyer_of_terrain(void) const { return ref_base_entity().flag_destroys_terrain; }
	bool is_vampiric(void) const { return ref_base_entity().flag_is_vampiric; }
	bool flag_follow_target = true;
	bool flag_stop_on_collision = true;


	void revive(void);






	//
	//
	void reset_targeting(void);
	void set_target_to_entityid(size_t const entityid);
	bool is_ready_to_attack(void) const;



	int get_life_max() const { return ref_base_entity().stat_life_max; }
	void set_life_to_max(void) { stat_life = get_life_max(); }


	int defense_bonus = 0;
	int get_defense() const { return(ref_base_entity().defense_base + defense_bonus); }
	int get_attack_base() const    { return ref_base_entity().attack_base; }
	int get_attack_dice() const    { return ref_base_entity().attack_dice; }
	int get_attack_maximum() const { return get_attack_base()+(1+get_attack_dice()); }

	int last_combat_attack_roll = -1;
	int last_combat_attack_damage = -1;



	// combat
	int combat_get_range(void) const;
	int combat_roll_damage(void);
	int combat_roll_damage_against_defense(int const defense);
	void take_damage(int const damage_to_take);
	void take_damage_as_fraction_of_current(int const multiply ,int const divide);
	void take_damage_as_fraction_of_max(int const multiply ,int const divide);
	void modify_life(int const delta_life);

	bool is_ready_to_move(void) const ;
	bool is_ready_to_cast_ability(void) const ;
	bool is_alive(void) const ;
	bool is_dead(void) const ;
	bool is_fully_decayed(void) const ;
	bool is_corpse(void) const ;
	int  ncurses_get_symbol(void) const ;
	int ncurses_get_attr_life(void) const ;
	bool is_renderable(void) const ;
	bool is_heavily_damaged(void) const ;
	bool is_slowed(void) const;


	int wprint_with_additional_attrs(
		 WINDOW * w
		,int const attrs_additional
		) const;


// timer
	void update_with_seconds(double const deltatime_seconds);


// general

	bool is_blocking(void) const;



	// movement
private:
	enum DIRECTION direction = DIRECTION_NONE;
	enum DIRECTION direction_persistent = DIRECTION_NONE;

public:
	enum DIRECTION direction_persistent_ai = DIRECTION_NONE;
	void set_direction_persistent(enum DIRECTION const dir);
	void set_direction_order(enum DIRECTION const dir) ;

	// ncurses ui
	void wprint_detailed_entity_info(WINDOW * w) const;
	char const * get_name(void) const;
	int ncurses_get_attrs(void) const;
	void order_stop(void);

	void update_movement_from_planned_movement(void);

	void update_movement(void);

	void position_restore_last(void) ;

	void update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER);

	bool has_planned_movement(void);
	void consume_food(void);


	int fprint_as_tsv_row(FILE * f);
	int fscan_as_tsv_row(FILE * f);

	void make_entity_use_healing_ability_id(size_t const id)
	{
		assert(is_ready_to_cast_ability());
		assert(id < vector_of_abilities.size());
		Ability& ref_ability = vector_of_abilities.at(id);
		int const deltalife = ref_ability.roll_and_consume();
		modify_life(deltalife);
		timer_ability.reset();
	}

	int make_entity_roll_ability_id(size_t const id)
	{
		assert(is_ready_to_cast_ability());
		assert(id < vector_of_abilities.size());
		Ability& ref_ability = vector_of_abilities.at(id);
		timer_ability.reset();
		return(ref_ability.roll_and_consume());
	}

	void wprint_entitylist_row(WINDOW * w,bool const is_target) const;

	void wprint_lifebar_at_the_rightmost(
		 WINDOW * w) const ;

	void wprint_detailed_entity_info_enemy(WINDOW * w) const;
};





