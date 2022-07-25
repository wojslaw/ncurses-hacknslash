#pragma once
// TODO prevent stacking of non-collision monsters

#include "Timer.hpp"
#include "Entity.hpp"
#include "VisualEntity.hpp"
#include "Vec2d.hpp"
#include "Ncurses.hpp"
#include "CollisionTable.hpp"

#include <assert.h>
#include <algorithm>
#include <vector>


typedef struct Level Level;
typedef struct LevelCell LevelCell;
typedef size_t IDEntity;
constexpr IDEntity IDEntity_invalid = -1;


#define FLAG_PRINT_ENTITYLIST_DEBUG  false
#define DISTANCE_FOLLOW_MAX_BEFORE_TARGET_LOST  20
#define SORT_BY_DISTANCE true



// IDEA optimize rendering by only updating after enough time occured, or something moved
// IDEA smart pointers in std::vector<Entity>





enum CellTerrain {
	CELLTERRAIN_NONE = 0 ,
	CELLTERRAIN_WARNING          ,
	CELLTERRAIN_RUBBLE           ,
	CELLTERRAIN_HASH             ,
	CELLTERRAIN_WALL_HORIZONTAL  ,
	CELLTERRAIN_WALL_VERTICAL    ,
	CELLTERRAIN_REWARD ,

	CELLTERRAIN_COUNT ,
};


// why doesn't this work?? ACS_HLINE gets drawn as ^0
// aah I think I know - ACS_ get initialized or something only after ncurses is started
	const chtype
TABLE_CELLTERRAIN_SYMBOL[CELLTERRAIN_COUNT] = {
		[CELLTERRAIN_NONE            ] = ' ' ,
		[CELLTERRAIN_WARNING         ] = '.' ,
		[CELLTERRAIN_RUBBLE          ] = '~' ,
		[CELLTERRAIN_HASH            ] = '#' ,
		[CELLTERRAIN_WALL_HORIZONTAL ] = (chtype)ACS_HLINE ,
		[CELLTERRAIN_WALL_VERTICAL   ] = (chtype)ACS_VLINE ,
		[CELLTERRAIN_REWARD          ] = '$' ,
};











struct LevelCell {
	enum CellTerrain cellterrain = CELLTERRAIN_NONE;
	bool has_entity = false;
	IDEntity id_of_entity = IDEntity_invalid;
	VisualEntity const * ptr_visual_entity = 0;

	void clear(void);

	bool is_cell_blocked_by_entity(void) const;
	bool is_cell_blocked_by_terrain(void) const;
	bool is_cell_blocked(void) const;
	bool is_cell_walkable(void) const;

	void set_cell_terrain_if_empty(enum CellTerrain const _cellterrain) ;
	void set_blocked_by_entity(IDEntity id);


	// mutating methods
	void damage_this_cell(void);

	void wrender(WINDOW * w) const;
	void wrender_move(WINDOW * w
			,int const y
			,int const x
			) const ;

	void wprint_detailed_info(WINDOW * w);
};






enum FeatureType {
	FEATURETYPE_ZERO = 0 ,
	FEATURETYPE_ROCK_INFESTED ,

	FEATURETYPE_COUNT ,
};



struct PendingLevelFeature {
	bool is_generated = false;
	enum FeatureType type = FEATURETYPE_ZERO;
	int size = 0;
	int start_y = 0;
	int start_x = 0;
	unsigned seed = 0;
	CountdownTimer timer;
	//ctor
	PendingLevelFeature(
			 enum FeatureType const _type
			,int const _size
			,int const _start_y
			,int const _start_x 
			,double const _seconds_of_countdown
			,unsigned const _seed
			)
		:type(_type)
		,size(_size)
		,start_y(_start_y)
		,start_x(_start_x)
		,seed(_seed)
		,timer(CountdownTimer(_seconds_of_countdown))
	{ }

	void update_time_from_globaltimer( GlobalTimer const & global_timer) 
	{
		timer.update_time_from_globaltimer(global_timer);
	}
};







struct Level {
	int y_max = 1;
	int x_max = 1;
	int get_highest_y(void) const {return y_max-1;}
	int get_highest_x(void) const {return x_max-1;}
	int get_size_y(void) const {return y_max;}
	int get_size_x(void) const {return x_max;}

	double total_seconds = 0.0;
	CountdownTimer timer_ai = CountdownTimer(2.0); // random number of seconds for countdown
	CountdownTimer timer_create_new_enemy = CountdownTimer(20.0);

	unsigned seed = 0;

	Level(
		 int const _y_max 
		,int const _x_max 
			);

	int feature_size_roll_base = 8;
	int feature_size_roll_dice = 8;



	std::vector<std::vector<LevelCell>> table_of_cells = std::vector<std::vector<LevelCell>>();
	std::vector<Entity> vector_of_entity = std::vector<Entity>();
	std::vector<VisualEntity> vector_of_visual_entity = std::vector<VisualEntity>();
	std::vector<size_t> vector_of_entityids_on_screen = std::vector<size_t>();
	std::vector<PendingLevelFeature> vector_of_pending_level_features;
	struct CollisionTable collision_table = CollisionTable(0,0);
	void update_collision_table(void);
	int get_count_of_living_entities(void) const;

	Entity const & constref_player_entity(void) const { return vector_of_entity.at(0); }
	Entity & ref_player_entity(void) { return vector_of_entity.at(0); }
	Entity & ref_from_entityid(size_t entityid) { return vector_of_entity.at(entityid); }
	Entity & ref_target(void) { return ref_from_entityid(ref_player_entity().id_of_target); }
	Entity & ref_from_visibleid(size_t const visibleid) { return vector_of_entity.at(vector_of_entityids_on_screen.at(visibleid)); }
	size_t entityid_from_visibleid(size_t const visibleid) const { return vector_of_entityids_on_screen.at(visibleid); }
	size_t visibleid_from_entityid(size_t const entityid) const;

	void player_tab_target(int const delta);
	void player_set_target_to_visibleid(int const visibleid);



	LevelCell &
		ref_levelcell_at_yx(
				 int const y
				,int const x
				);
	LevelCell &
		ref_levelcell_at_vec2d(
				Vec2d const & v
				);
	std::vector<IDEntity> get_targetable_entities_around_point_with_range_skip_player(
			 Vec2d const& point
			,int const range
			) const;
	bool is_position_within_bounds_of_level_yx(int const y, int const x) const;
	bool is_position_within_bounds_of_level_vec2d(Vec2d const & v) const;
	bool is_vec2d_position_within_rectangle(
			Vec2d const & v
			,int const y_start
			,int const x_start
			,int const y_max
			,int const x_max
			) const;
	void ensure_vec2d_position_is_within_bounds(Vec2d * v);



	void level_add_terrain_feature_noise_with_cellterain_global(
			CellTerrain cellterrain
			,unsigned const randomness_dice
			,unsigned randomness_seed
			);

	void level_add_terrain_feature_noise_with_cellterain_local(
			CellTerrain cellterrain
			,int const start_y
			,int const start_x
			,int const end_y
			,int const end_x
			,unsigned const randomness_dice
			,unsigned randomness_seed
			);

	void level_add_terrain_feature_rectangle_with_cellterain_fill(
			 CellTerrain const cellterrain
			,int const _start_y
			,int const _start_x
			,int const _end_y
			,int const _end_x
			);

	void level_add_terrain_feature_rectangle_with_cellterrain_border(
			CellTerrain const cellterrain
			,int const _start_y
			,int const _start_x
			,int const _end_y
			,int const _end_x
			);

	void level_add_terrain_feature_rectangle_room(
			 int const _start_y
			,int const _start_x
			,int const _end_y
			,int const _end_x
			);


	void level_terrain_clear_around_player(void) ;

	void level_create_terrain_messy_hall(void);

	void level_create_terrain_open_field(
			 int const rock_amount
			,int const rock_size_base
			,int const rock_size_dice
			,int const rubble_dice
			,unsigned randomness_seed 
			);

	void make_player_use_ability_id_autotarget(int const id);

	bool handle_input_mouse(
		 WINDOW * w
		,int y
		,int x
		) ;

//private:
	void remove_decayed_entities(void);
	void move_decayed_entities(void);

	void update_entity_combat_rounds(void);
	void update_table_of_cells_with_pointers_to_entities(void) ;


	void stop_targeting_if_player_is_dead(void);
	void make_visible_enemies_start_targeting_player(void);
	void update_vector_of_entityids_with_entities_within_rectangle(
		 int const y_start
		,int const x_start
		,int const y_end
		,int const x_end );

	void update_vector_of_entityids_with_entities_within_range_of_player(int const range_of_search);


//public:

	void update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER);//TODO

	// bool is_position_occupied_vec2d(Vec2d const& vec2d);

	void update_entities_positions(void) ;
	void update_entities_direction_planned(void) ;

	void delete_decayed_entities_if_player_has_no_target(void);


	void wrender_level_from_position_fill_window(
			WINDOW * w
			,int const pos_inlevel_start_y
			,int const pos_inlevel_start_x
			);

	void wrender_level_centered_on_player_entity_fill_window(
			WINDOW * w
			);


	void wprint_entitylist(
				WINDOW * w);



	void make_visual_effect_on_point_vec2d(
			 Vec2d const v_base
			,int const range
			);


	void make_visual_effect_on_point_yx(
			 int y
			,int x
			,int const range
			);

	void make_visual_effect_on_target(int const range);
	void make_visual_effect_on_player(int const range);

	void create_random_enemy_group(void);
	void ensure_entities_are_within_bounds(void);

	void roll_new_random_feature(void);
	void generate_level_feature_id(size_t const id);

	void handle_input_mouse(
			 WINDOW * w
			,int y
			,int x
			,mmask_t bstate
			);

	bool make_player_select_target_at_position(
			Vec2d const& position
			);

	Vec2d vec2d_position_from_window_mouse(
			 WINDOW * w
			,int y
			,int x
			) const;

	void make_entityid_use_ability_id_on_position(
		 IDEntity identity
		,int const id_ability
		,Vec2d const& position
		);




}; // struct Level










