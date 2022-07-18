#pragma once

#include "Timer.hpp"
#include "Entity.hpp"
#include "Vec2d.hpp"

#include <assert.h>
#include <vector>
#include <ncurses.h>


typedef struct Level Level;
typedef struct LevelCell LevelCell;






// IDEA optimize rendering by only updating after enough time occured, or something moved
// IDEA smart pointers in std::vector<Entity>



enum CellTerrain {
	CELLTERRAIN_NONE = 0 ,
	CELLTERRAIN_HASH ,
	CELLTERRAIN_WALL_HORIZONTAL ,
	CELLTERRAIN_WALL_VERTICAL ,
};

int const NCURSES_TABLE_CELLTERRAIN_SYMBOL[] = {
	[CELLTERRAIN_NONE] = '.' ,
	[CELLTERRAIN_HASH] = '#' ,
	[CELLTERRAIN_WALL_HORIZONTAL] = '-' ,
	[CELLTERRAIN_WALL_VERTICAL]   = '|' ,
};



struct LevelCell {
	enum CellTerrain cellterrain = CELLTERRAIN_NONE;
	Entity const * ptr_entity = 0;
	size_t id_of_entity = 0;

	bool is_blocked_cell(void) const {
		if(cellterrain != CELLTERRAIN_NONE) {
			return true;
		}
		return false;
		if(ptr_entity) {
			return ptr_entity->is_blocking();
		}
		return false;
	}
};











struct Level {
	int y_max;
	int x_max;

	double total_seconds = 0.0;

	Level(
		 int const _y_max 
		,int const _x_max 
			)
	{
		// player entity:
		vector_of_entity.resize(1);
		//
		y_max = _y_max;
		x_max = _x_max;
		table_of_cells.resize(y_max);
		int y = 0;
		for(auto & row : table_of_cells ) {
			row.resize(x_max);
			int x = 0;
			for(auto & cell : row) {
				if(x == 0 || x == x_max-1 || y == 0 || y == y_max-1) {
					cell.cellterrain = CELLTERRAIN_HASH;
				}
				++x;
			}
			++y;
		}
	}
	Entity & ref_player_entity(void) { return vector_of_entity.at(0); }

	std::vector<std::vector<LevelCell>> table_of_cells;
	std::vector<Entity> vector_of_entity;

	//TODO update_with_seconds(); or from_globaltimer

	LevelCell &
		ref_levelcell_at_yx(
				 int const y
				,int const x
				);
	LevelCell &
		ref_levelcell_at_vec2d(
				Vec2d const & v
				);
	bool is_vec2d_position_within_bounds(Vec2d const & v) const;
	void ensure_vec2d_position_is_within_bounds(Vec2d * v);



//private:
	void update_table_of_cells_with_pointers_to_entities(void) ;
	void update_entities(void) {
		for(auto & ref_entity : vector_of_entity ) {
			//ref_entity.update_position();
			ensure_vec2d_position_is_within_bounds(&(ref_entity.vec2d_position));
		}
		update_table_of_cells_with_pointers_to_entities();
		for(auto & ref_entity : vector_of_entity ) {
			LevelCell & cell_at_new_position = ref_levelcell_at_vec2d(ref_entity.vec2d_position);
			if(cell_at_new_position.is_blocked_cell()) {
				ref_entity.position_restore_last();
			}
		}
	}

//public:

	void update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER);//TODO

	void
		wprint_range(
				WINDOW * w
				,int const y_start
				,int const x_start
				,int const y_end
				,int const x_end
				) const ;
};




	void 
Level::update_table_of_cells_with_pointers_to_entities(void) 
{
	// clean
	table_of_cells.resize(y_max);
	for(auto & row : table_of_cells ) {
		for( auto & cell : row ) {
			cell.ptr_entity = 0;
		}
	}
	vector_of_entity.resize(1);
	// and add
	size_t id_of_entity = 0;
	for(const auto & entity : vector_of_entity) {
		// skip invalid-positioned entities
		if(entity.vec2d_position.y < 0) {
			continue;
		}
		if(entity.vec2d_position.x < 0) {
			continue;
		}
		if(entity.vec2d_position.x >= x_max) {
			continue;
		}
		if(entity.vec2d_position.y >= y_max) {
			continue;
		}
		//
		LevelCell & levelcell = ref_levelcell_at_vec2d(entity.vec2d_position);
		levelcell.ptr_entity = &entity;
		levelcell.id_of_entity = id_of_entity;
		//
		++id_of_entity;
	}
}




	LevelCell &
Level::ref_levelcell_at_yx(
		 int const y
		,int const x
		)
{
	;// TODO errorcheck
	return table_of_cells.at(y).at(x);
}


	LevelCell &
Level::ref_levelcell_at_vec2d(
		Vec2d const & v
		)
{
	// TODO errorcheck
	return table_of_cells.at(v.y).at(v.x);
}



	bool
Level::is_vec2d_position_within_bounds(Vec2d const & v) const
{
	if(v.y < 0) {
		return false;
	}
	if(v.x < 0) {
		return false;
	}
	if(v.x >= x_max) {
		return false;
	}
	if(v.y >= y_max) {
		return false;
	}
	return true;
}




	void
Level::ensure_vec2d_position_is_within_bounds(Vec2d * v)
{
	assert(v);
	if(!v) {
		return;
	}
	if(v->y < 0) {
		v->y = 0;
	}
	if(v->x < 0) {
		v->x = 0;
	}
	if(v->y >= y_max) {
		v->y = y_max-1;
	}
	if(v->x >= x_max) {
		v->x = x_max-1;
	}
}







	void
Level::wprint_range(
		 WINDOW * w
		,int y_start
		,int x_start
		,int y_end
		,int x_end
		) const
{

	assert(w);
	// ensure is within range
	if(y_start < 0) {
		y_start = 0;
	}
	if(x_start < 0) {
		x_start = 0;
	}
	if(y_end >= y_max) {
		y_end = y_max-1;
	}
	if(x_end >= x_max) {
		x_end = x_max-1;
	}


	// render into window
	wmove(w,0,0);
	for(int y = y_start; y <= y_end; ++y ) {
		for(int x = x_start; x <= x_end; ++x ) {
			const auto & ref_levelcell = table_of_cells.at(y).at(x);
			if(ref_levelcell.ptr_entity) {
				// render entity
				waddch(w,ref_levelcell.ptr_entity->ncurses_symbol);
			} else {
				//otherwise try to render as terrain
				waddch(w,NCURSES_TABLE_CELLTERRAIN_SYMBOL[ref_levelcell.cellterrain]);
			}
		}
		waddch(w,'\n');
	}
	wrefresh(w);
}








	void
Level::update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER)
{
	total_seconds += GLOBALTIMER.deltatime_seconds;
	for(Entity & entity : vector_of_entity) {
		entity.update_time_from_globaltimer(GLOBALTIMER);
	}
	update_table_of_cells_with_pointers_to_entities();
	update_entities();
}
