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


#define FLAG_PRINT_ENTITYLIST_DEBUG  false
#define DISTANCE_FOLLOW_MAX_BEFORE_TARGET_LOST  20



// IDEA optimize rendering by only updating after enough time occured, or something moved
// IDEA smart pointers in std::vector<Entity>



enum CellTerrain {
	CELLTERRAIN_NONE = 0 ,
	CELLTERRAIN_RUBBLE ,
	CELLTERRAIN_HASH ,
	CELLTERRAIN_WALL_HORIZONTAL ,
	CELLTERRAIN_WALL_VERTICAL ,
	CELLTERRAIN_CORPSE ,
};

int const NCURSES_TABLE_CELLTERRAIN_SYMBOL[] = {
	[CELLTERRAIN_NONE] = ' ' ,
	[CELLTERRAIN_RUBBLE] = '+' ,
	[CELLTERRAIN_HASH] = '#' ,
	[CELLTERRAIN_WALL_HORIZONTAL] = '-' ,
	[CELLTERRAIN_WALL_VERTICAL]   = '|' ,
	[CELLTERRAIN_CORPSE]   = '%' ,
};



struct LevelCell {
	enum CellTerrain cellterrain = CELLTERRAIN_NONE;
	Entity const * ptr_entity = 0;
	size_t id_of_entity = 0;
	VisualEntity const * ptr_visual_entity = 0;

	bool is_blocked_cell(void) const ;
	bool is_cell_walkable(void) const ;

	// mutating methods
	void damage_this_cell(void);

	int wprint(WINDOW * w) const;
	int mvwprint(WINDOW * w
			,int const y
			,int const x
			) const ;
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

	unsigned seed = 0;

	Level(
		 int const _y_max 
		,int const _x_max 
			);



	std::vector<std::vector<LevelCell>> table_of_cells = std::vector<std::vector<LevelCell>>();
	std::vector<Entity> vector_of_entity = std::vector<Entity>();
	std::vector<VisualEntity> vector_of_visual_entity = std::vector<VisualEntity>();
	std::vector<size_t> vector_of_entityids_on_screen = std::vector<size_t>();
	struct CollisionTable collision_table = CollisionTable(0,0);
	void update_collision_table(void);
	int get_count_of_living_entities(void) const;

	Entity & ref_player_entity(void) { return vector_of_entity.at(0); }
	Entity & ref_from_entityid(size_t const entityid) { return vector_of_entity.at(entityid); }
	Entity & ref_target(void) { return ref_from_entityid(ref_player_entity().id_of_target); }
	Entity & ref_from_visibleid(size_t const visibleid) { return vector_of_entity.at(vector_of_entityids_on_screen.at(visibleid)); }
	size_t entityid_from_visibleid(size_t const visibleid) const { return vector_of_entityids_on_screen.at(visibleid); }
	size_t visibleid_from_entityid(size_t const entityid) const;

	void player_tab_target();
	void player_set_target_to_visibleid_from_digit(int const input_digit);



	LevelCell &
		ref_levelcell_at_yx(
				 int const y
				,int const x
				);
	LevelCell &
		ref_levelcell_at_vec2d(
				Vec2d const & v
				);
	bool is_position_within_bounds_of_level_yx(int const y, int const x) const;
	bool is_vec2d_position_within_bounds_of_level(Vec2d const & v) const;
	bool is_vec2d_position_within_rectangle(
			Vec2d const & v
			,int const y_start
			,int const x_start
			,int const y_max
			,int const x_max
			) const;
	void ensure_vec2d_position_is_within_bounds(Vec2d * v);



	void level_add_terrain_feature_noise_with_cellterain_(
			CellTerrain cellterrain
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


	void level_terrain_clear_around_player(void) ;

	void level_create_terrain_messy_hall(void);

	void level_create_terrain_open_field(
			 int const rock_amount
			,int const rock_size_base
			,int const rock_size_dice
			,int const rubble_dice
			,unsigned randomness_seed 
			);


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

	void update_entities(void) ;
	void update_entities_direction_planned(void) ;


	void wprint_render_from_position_fill_window(
			WINDOW * w
			,int const pos_inlevel_start_y
			,int const pos_inlevel_start_x
			);

	void wprint_render_centered_on_player_entity_fill_window(
			WINDOW * w
			);


	void wprint_entitylist(
				WINDOW * w);


	void make_visual_effect_on_target(int const range);
	void make_visual_effect_on_player(int const range);
}; // struct Level


