#pragma once

#include "Timer.hpp"
#include "Entity.hpp"
#include "VisualEntity.hpp"
#include "Vec2d.hpp"
#include "Ncurses.hpp"

#include <assert.h>
#include <vector>


typedef struct Level Level;
typedef struct LevelCell LevelCell;


bool FLAG_PRINT_ENTITYLIST_DEBUG = false;




// IDEA optimize rendering by only updating after enough time occured, or something moved
// IDEA smart pointers in std::vector<Entity>



enum CellTerrain {
	CELLTERRAIN_NONE = 0 ,
	CELLTERRAIN_GRASS ,
	CELLTERRAIN_HASH ,
	CELLTERRAIN_WALL_HORIZONTAL ,
	CELLTERRAIN_WALL_VERTICAL ,
	CELLTERRAIN_CORPSE ,
};

int const NCURSES_TABLE_CELLTERRAIN_SYMBOL[] = {
	[CELLTERRAIN_NONE] = ' ' ,
	[CELLTERRAIN_GRASS] = '+' ,
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

	bool is_blocked_cell(void) const {
		if(cellterrain != CELLTERRAIN_NONE) {
			return true;
		}
		return false;
	}
};











struct Level {
	int y_max = 1;
	int x_max = 1;

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
	bool is_vec2d_position_within_bounds_of_level(Vec2d const & v) const;
	bool is_vec2d_position_within_rectangle(
			Vec2d const & v
			,int const y_start
			,int const x_start
			,int const y_max
			,int const x_max
			) const;
	void ensure_vec2d_position_is_within_bounds(Vec2d * v);



//private:
	void remove_decayed_entities(void);
	void move_decayed_entities(void);

	void update_entity_combat_rounds(void);
	void update_table_of_cells_with_pointers_to_entities(void) ;

	void update_vector_of_entityids_on_screen_within_range(
		 int const y_start
		,int const x_start
		,int const y_end
		,int const x_end );


//public:

	void update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER);//TODO

	void update_entities(void) ;

	void
		wprint_range(
				WINDOW * w
				,int const y_start
				,int const x_start
				,int const y_end
				,int const x_end
				) ;

	void
		wprint_centered_on_player_entity_with_window_halfsize(
		 WINDOW * w
		,int y_halfsize
		,int x_halfsize
		) ;


	void wprint_entitylist(
				WINDOW * w);


	void make_visual_effect_on_target(int const range);
}; // struct Level




	void 
Level::update_table_of_cells_with_pointers_to_entities(void) 
{
	// clean
	for(auto & row : table_of_cells ) {
		for( auto & cell : row ) {
			cell.ptr_entity = 0;
			cell.id_of_entity = 0;
			cell.ptr_visual_entity = 0;
		}
	}
	// and add
	size_t id_of_entity = 0;
	for(const auto & entity : vector_of_entity) {
		// skip invalid-positioned entities
		if(!(is_vec2d_position_within_bounds_of_level(entity.vec2d_position))) {
			continue;
		}
		//
		LevelCell & levelcell = ref_levelcell_at_vec2d(entity.vec2d_position);
		levelcell.ptr_entity = &entity;
		levelcell.id_of_entity = id_of_entity;
		//
		++id_of_entity;
	}

	for(const auto & visual_entity : vector_of_visual_entity) {
		if(!(visual_entity.is_valid())) {
			continue;
		}
		// skip invalid-positioned entities
		if(!(is_vec2d_position_within_bounds_of_level(visual_entity.vec2d_position))) {
			continue;
		}
		//
		LevelCell & levelcell = ref_levelcell_at_vec2d(visual_entity.vec2d_position);
		levelcell.ptr_visual_entity = &visual_entity;
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
	assert(v.y >= 0);
	assert(v.x >= 0);
	assert(is_vec2d_position_within_bounds_of_level(v));
	return table_of_cells.at(v.y).at(v.x);
}



	bool
Level::is_vec2d_position_within_bounds_of_level(Vec2d const & v) const
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
		)
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
	werase(w);
	box(w,0,0);
	wmove(w,1,1);
	// render terrain
	for(int y = y_start; y <= y_end; ++y ) {
		for(int x = x_start; x <= x_end; ++x ) {
			const auto & ref_levelcell = table_of_cells.at(y).at(x);
			if(ref_levelcell.ptr_entity) { // render entity //playing with fire, dereferencing a pointer before checking xD
				if( ref_levelcell.ptr_entity->ncurses_get_symbol() != 0 ) {
					int attrs = ref_levelcell.ptr_entity->ncurses_get_attrs();
					if(ref_levelcell.ptr_entity == &ref_from_entityid(ref_player_entity().id_of_target)) {
						attrs = attrs | WA_REVERSE;
					}
					if(attrs != 0) { wattron(w,attrs); }
					waddch(w,ref_levelcell.ptr_entity->ncurses_get_symbol());
					if(attrs != 0) { wattroff(w,attrs); }
				} else {
					wmove(w,getcury(w),getcurx(w));
				}
			} else if(ref_levelcell.ptr_visual_entity){ // visual entity
				int attrs = ref_levelcell.ptr_visual_entity->ncurses_attrs;
				if(attrs != 0) { wattron(w,attrs); }
				waddch(w,ref_levelcell.ptr_visual_entity->ncurses_symbol);
				if(attrs != 0) { wattroff(w,attrs); }
			} else {
				//otherwise try to render as terrain
				wattron(w,ATTR_TERRAIN);
				waddch(w,NCURSES_TABLE_CELLTERRAIN_SYMBOL[ref_levelcell.cellterrain]);
				wattroff(w,ATTR_TERRAIN);
			}
		}
		wmove(w,getcury(w)+1,1);
	}
	update_vector_of_entityids_on_screen_within_range(
		 y_start
		,x_start
		,y_end
		,x_end );
	// end
	wrefresh(w);
}



	void
Level::wprint_centered_on_player_entity_with_window_halfsize(
		 WINDOW * w
		,int const y_halfsize
		,int const x_halfsize
		)
{
	assert(w);
	const Vec2d & center_of_screen = ref_player_entity().vec2d_position;
	int const y_start = center_of_screen.y - y_halfsize;
	int const x_start = center_of_screen.x - x_halfsize;
	int const y_end   = center_of_screen.y + y_halfsize;
	int const x_end   = center_of_screen.x + x_halfsize;
	// NODO try to fill the given size screen - I decided that it's better to keep the screensize smaller
	// It would be weird if at the corner you'd see more xD
	wprint_range(
			w
			,y_start 
			,x_start 
			,y_end   
			,x_end   );
}








	void
Level::update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER)
{
	total_seconds += GLOBALTIMER.deltatime_seconds;
	// AI timer
	timer_ai.update_time_from_globaltimer(GLOBALTIMER);
	int const ai_tick = timer_ai.consume_all_ticks();
	if(ai_tick > 0){
		// change direction
		for(size_t id = 1 ; id <= 3; ++ id ) {
			Entity & entity = vector_of_entity.at(id);
			switch(entity.direction_persistent_ai) {
				case DIRECTION_UP:
					entity.direction_persistent_ai = DIRECTION_RIGHT;
					break;
				case DIRECTION_RIGHT:
					entity.direction_persistent_ai = DIRECTION_DOWN;
					break;
				case DIRECTION_DOWN:
					entity.direction_persistent_ai = DIRECTION_LEFT;
					break;
				default:
					entity.direction_persistent_ai = DIRECTION_UP;
					break;
			}
		}
	}
	// visual entities
	mvprintw(LINES-2,0,"visual entities update_time_from_globaltimer\n");
	for(VisualEntity & visual_entity : vector_of_visual_entity) {
		visual_entity.update_time_from_globaltimer(GLOBALTIMER);
		// TODO delete useless visual entities
	}

	mvprintw(LINES-2,0,"move_decayed_entities\n");
	move_decayed_entities();
	// remove_decayed_entities();

	// entities
	mvprintw(LINES-2,0,"entities update_time_from_globaltimer\n");
	for(Entity & entity : vector_of_entity) {
		entity.update_time_from_globaltimer(GLOBALTIMER);
		// remove decayed
	}
	// 
	mvprintw(LINES-2,0,"update_table_of_cells_with_pointers_to_entities\n");
	update_table_of_cells_with_pointers_to_entities();
	mvprintw(LINES-2,0,"update_entities\n");
	update_entities();
	update_entity_combat_rounds();
}






	void
Level::wprint_entitylist(
		WINDOW * w)
{
	assert(w);
	werase(w);
	wmove(w,0,0);
	int const max_entities_to_print = getmaxy(w)-(2+1); // +2 for borders, +1
	int const max_line_length = getmaxx(w)-(2+2); // +2 for borders, +2 for ".."
	int count_of_printed = 0;
	size_t visibleid = 0;
	for(size_t id : vector_of_entityids_on_screen) {
		if(count_of_printed >= max_entities_to_print) {
			break;
		}
		//
		char buffer[0x100] = {0};
		int const bytes_written
			= FLAG_PRINT_ENTITYLIST_DEBUG
			? snprintf(
					buffer
					,max_line_length
					,"  %zu %c  (e%zu) \n"
					,visibleid
					,vector_of_entity.at(id).ncurses_symbol
					,id
					 )
			: snprintf(
					buffer
					,max_line_length
					,"  %c %2d/%2d\n"
					,vector_of_entity.at(id).ncurses_symbol
					,vector_of_entity.at(id).stat_life
					,vector_of_entity.at(id).stat_life_max
					 );
		//
		if(id == ref_player_entity().id_of_target ) {
			buffer[0] = '>';
		}
		int const y = 1 + count_of_printed;
		mvwaddstr(w,y,1,buffer);
		if(bytes_written >= max_line_length) {
			waddstr(w,"..");
		}
		++count_of_printed;
		++visibleid;
	}

	if(vector_of_entityids_on_screen.size() > 1) {
		if(count_of_printed < (int)vector_of_entityids_on_screen.size()-1) {
			wprintw(w,"...[%d more]"
					, (((int)vector_of_entityids_on_screen.size())-count_of_printed-1)
				   );
		}
	}

	wrefresh(w);
}






	size_t 
Level::visibleid_from_entityid(size_t const entityid) const
{
	for(    size_t visibleid = 0 ; 
	        visibleid < vector_of_entityids_on_screen.size() ;
	        ++visibleid ) {
		if(entityid_from_visibleid(visibleid) == entityid) {
			return visibleid;
		}
	}
	return 0;
}




	void
Level::player_tab_target()
{
	Entity & player_entity = ref_player_entity();
	if(vector_of_entityids_on_screen.size() <= 1 ) {
		player_entity.reset_targeting();
		return;
	}

	// find visibleid for entityid of target
	size_t const visibleid_of_target = visibleid_from_entityid(player_entity.id_of_target);
	size_t const visibleid_of_next_target = visibleid_of_target + 1;
	size_t const highest_visibleid = vector_of_entityids_on_screen.size() - 1;

	// set visible id
	if(visibleid_of_next_target <= highest_visibleid) {
		player_entity.set_target_to_entityid(entityid_from_visibleid(visibleid_of_next_target));
	} else {
		player_entity.set_target_to_entityid(1); // lowest expected visibleid
	}
}





	void
Level::player_set_target_to_visibleid_from_digit(int const input_digit)
{
	assert(input_digit >= 0);
	size_t const highest_visibleid = vector_of_entityids_on_screen.size();
	if((size_t)input_digit >= highest_visibleid) {
		ref_player_entity().id_of_target = highest_visibleid;
		return;
	}
	ref_player_entity().id_of_target = entityid_from_visibleid(input_digit);
}









	void
Level::update_entity_combat_rounds(void)
{
	for(int i = 0; i < (int)vector_of_entity.size(); ++i) {
		Entity & attacker = vector_of_entity.at(i);
		if(!(attacker.has_selected_target)) {
			continue;
		}
		if(!(attacker.is_ready_to_attack())) {
			continue;
		}
		Entity & target = ref_from_entityid(attacker.id_of_target);
		int const distance
			= vec2d_highest_distance_between(
					 target.vec2d_position
					,attacker.vec2d_position
					);
		// and within range
		if(distance > attacker.combat_get_range()) {
			continue;
		}
		// finally 
		target.take_damage(attacker.combat_roll_damage_against_defense(target.get_defense()));
	}
}




	void
Level::make_visual_effect_on_target(int const range)
{
	assert(range >= 1);
	const Vec2d & v_base = ref_from_entityid(ref_player_entity().id_of_target).vec2d_position;
	for(int y = -range ; y < range; ++y) {
		for(int x = -range ; x < range; ++x) {
			vector_of_visual_entity.push_back(
					VisualEntity(
						 'x'
						,0.25
						,v_base.y+y
						,v_base.x+x) );
		}
	}
}













	void
Level::remove_decayed_entities(void)
{
	// difficult to make it safely xD throws exceptions at weird moments // remove decayed corpses
	// maybe if I go from end to start, it will be less deadly
	for(size_t i = vector_of_entity.size()-1;
			i > 0;
			--i
	   ) {
		if(vector_of_entity.at(i).is_fully_decayed()) {
			vector_of_entity.erase(vector_of_entity.begin() + i);
		}
	}
}


	void
Level::move_decayed_entities(void)
{
	// difficult to make it safely xD throws exceptions at weird moments // remove decayed corpses
	// maybe if I go from end to start, it will be less deadly
	for(size_t i = 0;
			i < vector_of_entity.size();
			++i
	   ) {
		if(vector_of_entity.at(i).is_fully_decayed()) {
			vector_of_entity.at(i).vec2d_position.y = -1;
			vector_of_entity.at(i).vec2d_position.x = -1;
		}
	}
}










//ctor
Level::Level(
		 int const _y_max 
		,int const _x_max 
			)
{
	// player entity:
	vector_of_entity.resize(8);
	vector_of_entity.at(0).vec2d_position.y = 8;
	vector_of_entity.at(0).vec2d_position.x = 8;
	//
	vector_of_entity.at(1).ncurses_symbol = 'a';
	vector_of_entity.at(1).take_damage(100);
	vector_of_entity.at(2).ncurses_symbol = 'b';
	vector_of_entity.at(3).ncurses_symbol = 'c';
	vector_of_entity.at(4).ncurses_symbol = 'd';
	vector_of_entity.at(5).ncurses_symbol = 'e';
	vector_of_entity.at(6).ncurses_symbol = 'f';
	vector_of_entity.at(7).ncurses_symbol = 'g';
	//
	vector_of_entity.at(1).vec2d_position.y = 4;
	vector_of_entity.at(1).vec2d_position.x = 4;
	vector_of_entity.at(2).vec2d_position.y = 2;
	vector_of_entity.at(2).vec2d_position.x = 2;
	vector_of_entity.at(3).vec2d_position.y = 8;
	vector_of_entity.at(3).vec2d_position.x = 8;
	//
	vector_of_entity.at(4).vec2d_position.y = 2;
	vector_of_entity.at(5).vec2d_position.y = 3;
	vector_of_entity.at(6).vec2d_position.y = 4;
	vector_of_entity.at(7).vec2d_position.y = 5;
	//
	vector_of_entity.at(1).timer_movement.seconds_countdown = 0.75;
	vector_of_entity.at(2).timer_movement.seconds_countdown = 0.5;
	vector_of_entity.at(3).timer_movement.seconds_countdown = 0.5;
	//

	y_max = _y_max;
	x_max = _x_max;
	table_of_cells.resize(y_max);
	int y = 0;
	for(auto & row : table_of_cells ) {
		row.resize(x_max);
		int x = 0;
		for(auto & cell : row) {
			int const is_grass = rand_r(&seed)%0x20 == 0;
			if(is_grass) {
				cell.cellterrain = CELLTERRAIN_GRASS;
			}
			if(x == 0 || x == x_max-1 || y == 0 || y == y_max-1) {
				cell.cellterrain = CELLTERRAIN_HASH;
			}
			if((x % 5) == 0 && (y % 3) == 0 ) {
				cell.cellterrain = CELLTERRAIN_HASH;
			}
			++x;
		}
		++y;
	}
}










	void
Level::update_entities(void) {
	update_table_of_cells_with_pointers_to_entities();
	for(Entity & ref_entity : vector_of_entity ) {
		if(!(is_vec2d_position_within_bounds_of_level(ref_entity.vec2d_position))) { continue;  } // skip invalidly-placed
		LevelCell & cell_at_new_position = ref_levelcell_at_vec2d(ref_entity.vec2d_position);
		if(cell_at_new_position.is_blocked_cell()) {
			ref_entity.position_restore_last();
		}
		for(Entity const& ref_entity_2 : vector_of_entity ) {
			if(&ref_entity_2 == &ref_entity) { // skip check if same
				continue;
			}
			if(!(ref_entity_2.is_blocking())) {
				continue;
			}
			if(Vec2d_is_equal(ref_entity.vec2d_position ,ref_entity_2.vec2d_position )) {
				ref_entity.position_restore_last();
			}
		}
	}
	return;
	//
	for(auto & ref_entity : vector_of_entity ) {
		//ref_entity.update_position();
		ensure_vec2d_position_is_within_bounds(&(ref_entity.vec2d_position));
	}
	//
	update_table_of_cells_with_pointers_to_entities();
}














	void
Level::update_vector_of_entityids_on_screen_within_range(
		int const y_start
		,int const x_start
		,int const y_end
		,int const x_end )
{
	vector_of_entityids_on_screen.resize(0);
	bool is_target_on_screen = false;
	for(size_t id = 0; id < vector_of_entity.size(); ++id) {
		auto const & entity = vector_of_entity.at(id);
		if(entity.is_dead()) { //don't show corpses
			continue;
		}
		if(entity.vec2d_position.is_within_rectangle(y_start,x_start,y_end,x_end)) {
			vector_of_entityids_on_screen.push_back(id);
			if(id == vector_of_entity.at(0).id_of_target) {
				is_target_on_screen = true;
			}
		}
	}
	if(!is_target_on_screen) {
		ref_player_entity().reset_targeting();
	}
}
