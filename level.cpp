#include "Level.hpp"

#define SQUISH_FEATURES_WHEN_TOO_BIG true

#define MAKE_ENEMIES true


	bool 
LevelCell::is_blocked_cell(void) const
{
	if(cellterrain != CELLTERRAIN_NONE) {
		return true;
	}
	return false;
}



	bool 
LevelCell::is_cell_walkable(void) const
{
	if(cellterrain == CELLTERRAIN_NONE) {
		return true;
	}
	return false;
}




	void
LevelCell::damage_this_cell(void)
{
	if(cellterrain == CELLTERRAIN_NONE) {
		return;
	}
	if(cellterrain == CELLTERRAIN_RUBBLE) {
		cellterrain = CELLTERRAIN_NONE;
		return;
	}
	cellterrain = CELLTERRAIN_RUBBLE;
}


	int 
LevelCell::wprint(WINDOW * w) const
{
	assert(w);
	if(cellterrain == CELLTERRAIN_NONE) {
		return waddch(w,NCURSES_TABLE_CELLTERRAIN_SYMBOL[cellterrain]);
	}
	wattron(w,ATTR_TERRAIN);
	waddch(w,NCURSES_TABLE_CELLTERRAIN_SYMBOL[cellterrain]);
	wattroff(w,ATTR_TERRAIN);
	return OK;
}


	int 
LevelCell::mvwprint(WINDOW * w
			,int const y
			,int const x
			) const
{
	assert(w);
	wmove(w,y,x);
	return wprint(w);
}









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
	if(v.x > get_highest_y()) {
		return false;
	}
	if(v.y > get_highest_x()) {
		return false;
	}
	return true;
}


	bool
Level::is_vec2d_position_within_rectangle(
			Vec2d const & v
			,int const y_start
			,int const x_start
			,int const y_max
			,int const x_max
			) const
{
	
	if(v.y < y_start) { return false; }
	if(v.x < x_start) { return false; }
	if(v.x > x_max) { return false; }
	if(v.y > y_max) { return false; }
	return true;
}












	bool
Level::is_position_within_bounds_of_level_yx(int const y, int const x) const
{
	if(y < 0) {
		return false;
	}
	if(x < 0) {
		return false;
	}
	if(y > get_highest_y()) {
		return false;
	}
	if(x > get_highest_x()) {
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
	v->y = std::max(v->y,0);
	v->x = std::max(v->x,0);

	v->y = std::min(get_highest_y(),v->y);
	v->x = std::min(get_highest_x(),v->x);
}






	void
Level::wprint_render_from_position_fill_window(
		 WINDOW * w
		,int const pos_inlevel_start_y
		,int const pos_inlevel_start_x
		)
{
	assert(w);
	int const pos_inlevel_last_y = pos_inlevel_start_y + getmaxy(w)-2;
	int const pos_inlevel_last_x = pos_inlevel_start_x + getmaxx(w)-2;
	// 0 prepare window
	// 1 SFX
	// 2 terrain & out-of-map border
	// 3 corpses
	// 4 entities

	// 0 prepare window
	werase(w);


	{ // 2 terrain
		for(int pos_window_y = 0; pos_window_y < getmaxy(w); ++pos_window_y ) {
			for(int pos_window_x = 0; pos_window_x < getmaxx(w); ++pos_window_x ) {
				wmove(w,pos_window_y,pos_window_x);
				int const pos_level_y = pos_inlevel_start_y + pos_window_y;
				int const pos_level_x = pos_inlevel_start_x + pos_window_x;
				// out-of-map border
				if(!is_position_within_bounds_of_level_yx(pos_level_y,pos_level_x)) {
					wattron(w ,ATTR_OUT_OF_MAP_BOUNDS);
					waddch(w,' ');
					wattroff(w,ATTR_OUT_OF_MAP_BOUNDS);
					continue; // prevent fatal error(trying to read beyond vector's size
				}
				ref_levelcell_at_yx(pos_level_y,pos_level_x).wprint(w);
			}
		}
	} //terrain

	{ // 1 SFX
		for(VisualEntity const& visual_entity_rendered : vector_of_visual_entity ) {
			move(3,0);
			printw("[%2d,%2d]"
					,visual_entity_rendered.vec2d_position.y
					,visual_entity_rendered.vec2d_position.x
					);
			if(is_vec2d_position_within_rectangle(
						 visual_entity_rendered.vec2d_position
						,pos_inlevel_start_y
						,pos_inlevel_start_x
						,pos_inlevel_last_y
						,pos_inlevel_last_x
						)) {
				printw(" Y ");
				int const pos_window_y = visual_entity_rendered.vec2d_position.y - pos_inlevel_start_y;
				int const pos_window_x = visual_entity_rendered.vec2d_position.x - pos_inlevel_start_x;
				wmove(w,pos_window_y,pos_window_x);
				visual_entity_rendered.wprint(w);
			}
		}
	} //SFX

	{ // 4 entities
		size_t entityid_being_rendered = 0;
		for(Entity const& entity_rendered : vector_of_entity ) {
			move(0,0);
			printw("in level: [%2d , %2d],[%2d , %2d]\n"
					,pos_inlevel_start_y
					,pos_inlevel_start_x
					,pos_inlevel_last_y
					,pos_inlevel_last_x );
			if(is_vec2d_position_within_rectangle(
						 entity_rendered.vec2d_position
						,pos_inlevel_start_y
						,pos_inlevel_start_x
						,pos_inlevel_last_y
						,pos_inlevel_last_x
						)) {
				int const pos_window_y = entity_rendered.vec2d_position.y - pos_inlevel_start_y;
				int const pos_window_x = entity_rendered.vec2d_position.x - pos_inlevel_start_x;
				wmove(w,pos_window_y,pos_window_x);
				int const attrs_additional
					= (entityid_being_rendered == ref_player_entity().id_of_target)
					? ATTR_TARGET
					: 0;
				entity_rendered.wprint_with_additional_attrs(w,attrs_additional);
			}
			++entityid_being_rendered;
		}
	} //entities

	// TODO decouple calculations from rendering
	update_vector_of_entityids_on_screen_within_range(
		 pos_inlevel_start_y
		,pos_inlevel_start_x
		,pos_inlevel_last_y
		,pos_inlevel_last_x );
	make_visible_enemies_start_targeting_player();
	stop_targeting_if_player_is_dead();

	// done
	wrefresh(w);
}


















	void
Level::wprint_range(
		 WINDOW * w
		,int const y_start
		,int const x_start
		,int y_end
		,int x_end
		)
{
	assert(w);
	// ensure is within range
	//
	int const render_start_pos_y
		= std::max(
				0
				,y_start);

	int const render_start_pos_x
		= std::max(
				0
				,x_start);

	int const render_end_pos_y
		= std::min(
				get_highest_y()
				,y_end
				);
	int const render_leftover_y = y_end - render_end_pos_y;

	int const render_end_pos_x
		= std::min(
				get_highest_x()
				,x_end
				);
	int const render_leftover_x = x_end - render_end_pos_x;

	// render into window
	werase(w);
	box(w,0,0);
	wmove(w,1,1);
	// render terrain
	int last_rendered_window_y = -1;
	int last_rendered_window_x = -1;
	for(int y = render_start_pos_y; y <= render_end_pos_y; ++y ) {
		for(int x = render_start_pos_x; x <= render_end_pos_x; ++x ) {
			last_rendered_window_y = std::max(y,last_rendered_window_y);
			last_rendered_window_x = std::max(x,last_rendered_window_x);
			const auto & ref_levelcell = table_of_cells.at(y).at(x);
			if(ref_levelcell.ptr_entity) { // render entity
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
	// TODO decouple calculations from rendering
	update_vector_of_entityids_on_screen_within_range(
		 y_start
		,x_start
		,y_end
		,x_end );
	make_visible_enemies_start_targeting_player();
	stop_targeting_if_player_is_dead();
	// out-of map border
	if(render_leftover_x > 0) {
		for(int y = 0; y < getmaxy(w); ++y) {
			wmove(w,y,last_rendered_window_x);
			wattron(w,WA_REVERSE);
			waddch(w,' ');
			wattroff(w,WA_REVERSE);
		}
	}
	if(render_leftover_y > 0) {
		for(int x = 0; x < getmaxx(w); ++x) {
			wmove(w,last_rendered_window_y,x);
			wattron(w,WA_REVERSE);
			waddch(w,' ');
			wattroff(w,WA_REVERSE);
		}
	}
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
Level::wprint_render_centered_on_player_entity_fill_window(
		 WINDOW * w
		)
{
	assert(w);
	Vec2d const & center_of_screen = ref_player_entity().vec2d_position;
	box(w,0,0);
	int const y_halfsize = getmaxy(w)/2;
	int const x_halfsize = getmaxx(w)/2;

	int const y_start = center_of_screen.y - y_halfsize;
	int const x_start = center_of_screen.x - x_halfsize;
	// NODO try to fill the given size screen - I decided that it's better to keep the screensize smaller
	// It would be weird if at the corner you'd see more xD
	wprint_render_from_position_fill_window(
			w
			,y_start 
			,x_start );
}






	void
Level::update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER)
{
	total_seconds += GLOBALTIMER.deltatime_seconds;
	// TODO AI AI timer
	//
//	timer_ai.update_time_from_globaltimer(GLOBALTIMER);
//	int const ai_tick = timer_ai.consume_all_ticks();
//	if(ai_tick > 0){
//		// change direction
//		for(size_t id = 1 ; id <= 3; ++ id ) {
//			Entity & entity = vector_of_entity.at(id);
//			switch(entity.direction_persistent_ai) {
//				case DIRECTION_UP:
//					entity.direction_persistent_ai = DIRECTION_RIGHT;
//					break;
//				case DIRECTION_RIGHT:
//					entity.direction_persistent_ai = DIRECTION_DOWN;
//					break;
//				case DIRECTION_DOWN:
//					entity.direction_persistent_ai = DIRECTION_LEFT;
//					break;
//				default:
//					entity.direction_persistent_ai = DIRECTION_UP;
//					break;
//			}
//		}
//	}
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
	update_entities_direction_planned();
	update_entity_combat_rounds();
	update_collision_table();
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
					,vector_of_entity.at(id).ncurses_get_symbol()
					,id
					 )
			: snprintf(
					buffer
					,max_line_length
					,"  %c %2d/%2d\n"
					,vector_of_entity.at(id).ncurses_get_symbol()
					,vector_of_entity.at(id).stat_life
					,vector_of_entity.at(id).get_life_max()
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
Level::update_entities_direction_planned(void)
{
	for(int i = 0; i < (int)vector_of_entity.size(); ++i) {
		Entity & attacker = vector_of_entity.at(i);
		if( attacker.flag_skip_update ) {
			continue;
		}
		if( not(attacker.has_selected_target) ) {
			continue;
		}
		if( not(attacker.flag_follow_target) ) {
			continue;
		}
		Entity & target = ref_from_entityid(attacker.id_of_target);
		int const distance 
			= vec2d_highest_distance_between(
					attacker.vec2d_position
					,target.vec2d_position
					);
		if(distance >= attacker.combat_get_range()) {
			attacker.vec2d_planned_movement
				= vec2d_find_direction_vec2d_from_to(
						attacker.vec2d_position
						,target.vec2d_position
						);
		} else {
			attacker.vec2d_planned_movement.set_zero();
		}
		if(distance > DISTANCE_FOLLOW_MAX_BEFORE_TARGET_LOST) { // lose target
			attacker.reset_targeting();
		}
	}
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
		if(target.is_dead()) {
			attacker.reset_targeting();
		}
	}
}




	void
Level::make_visual_effect_on_target(int const range)
{
	assert(range >= 1);
	const Vec2d & v_base = ref_from_entityid(ref_player_entity().id_of_target).vec2d_position;
	for(int y = -range ; y <= range; ++y) {
		for(int x = -range ; x <= range; ++x) {
			vector_of_visual_entity.emplace_back(
					VisualEntity(
						 v_base.y+y
						,v_base.x+x) );
		}
	}
}


	void
Level::make_visual_effect_on_player(int const range)
{
	assert(range >= 1);
	const Vec2d & v_base = ref_player_entity().vec2d_position;
	for(int y = -range ; y <= range; ++y) {
		for(int x = -range ; x <= range; ++x) {
			vector_of_visual_entity.emplace_back(
					VisualEntity(
						 v_base.y+y
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




















	void
Level::update_entities(void) {
	update_table_of_cells_with_pointers_to_entities();
	for(Entity & ref_entity : vector_of_entity ) {
		if(!(is_vec2d_position_within_bounds_of_level(ref_entity.vec2d_position))) { continue;  } // skip invalidly-placed
		LevelCell & cell_at_new_position = ref_levelcell_at_vec2d(ref_entity.vec2d_position);
		if(!ref_entity.has_collision()) {  // skip entities without collision
			continue;
		}
		if(cell_at_new_position.is_blocked_cell()) {
			if(ref_entity.has_destroyer_of_terrain()) { // try to destroy cell
				cell_at_new_position.damage_this_cell();
			}
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
	for(Entity & ref_entity : vector_of_entity ) {
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
			vector_of_entityids_on_screen.emplace_back(id);
			if(id == vector_of_entity.at(0).id_of_target) {
				is_target_on_screen = true;
			}
		}
	}
	if(!is_target_on_screen) {
		ref_player_entity().reset_targeting();
	}
}



	void
Level::make_visible_enemies_start_targeting_player(void)
{
	for( size_t entityid : vector_of_entityids_on_screen ) {
		Entity & entity = vector_of_entity.at(entityid);
		if(&entity == &ref_player_entity()) {
			continue; 
		}
		entity.set_target_to_entityid(0);
	}
}




void
Level::stop_targeting_if_player_is_dead(void)
{
	if(ref_player_entity().is_alive()) {
		return;
	}
	for( Entity & entity : vector_of_entity ) {
		entity.reset_targeting();
	}
}




	int
Level::get_count_of_living_entities(void) const
{
	int living = 0;
	for( auto const& entity : vector_of_entity ) {
		if(entity.is_alive()) {
			living++;
		}
	}
	return living;
}



	void
Level::update_collision_table(void)
{
	move(LINES-2,0);
	printw("update_collision_table");
	assert(y_max > 0);
	assert(x_max > 0);
	assert(table_of_cells.size() > 0);
	assert(table_of_cells.at(0).size() > 0);
	//assert(table_of_cells.size() == (size_t)y_max);
	//assert(table_of_cells.size() == (size_t)x_max);
	//
	collision_table.resize_and_clear(get_size_y(),get_size_x());
	// entities
	for(Entity const& entity : vector_of_entity ) {
		if(entity.is_blocking()) {
			collision_table.set_blocked_vec2d(entity.vec2d_position);
		}
	}
	//terrain
	for(int y = 0; y < y_max; ++y ) {
		for(int x = 0; x < x_max; ++x ) {
			if(not table_of_cells.at(y).at(x).is_cell_walkable()) {
				collision_table.set_blocked_yx(y,x);
			}
		}
	}
}







	void
Level::level_add_terrain_feature_noise_with_cellterain_(
		 CellTerrain cellterrain
		,unsigned const randomness_dice
		,unsigned randomness_seed
		)
{
	for(int y = 0; y < get_highest_y(); ++y) {
		for(int x = 0; x < get_highest_x(); ++x) {
			if(rand_r(&randomness_seed)%randomness_dice == 0) {
				ref_levelcell_at_yx(y,x).cellterrain = cellterrain;
			}
		}
	}
}




	void
Level::level_add_terrain_feature_rectangle_with_cellterain_fill(
		 CellTerrain const cellterrain
		,int _start_y
		,int _start_x
		,int _end_y
		,int _end_x
		)
{
	// squish if too large
	if(SQUISH_FEATURES_WHEN_TOO_BIG) {
		if(_start_y < 0) { _start_y = 0; }
		if(_start_x < 0) { _start_x = 0; }
		if(_end_y > get_highest_y()) { _end_y = get_highest_y(); }
		if(_end_x > get_highest_x()) { _end_x = get_highest_x(); }
	}

	assert(_start_y >=0);
	assert(_start_x >=0);
	assert(_end_y <= get_highest_y());
	assert(_end_x <= get_highest_x());

	for(int y = _start_y; y <= _end_y; ++y) {
		for(int x = _start_x; x <= _end_x; ++x) {
			ref_levelcell_at_yx(y,x).cellterrain = cellterrain;
		}
	}
}






	void
Level::level_add_terrain_feature_rectangle_with_cellterrain_border(
		 CellTerrain const cellterrain
		,int const _start_y
		,int const _start_x
		,int const _end_y
		,int const _end_x
		)
{
	assert(_start_y >=0);
	assert(_start_x >=0);
	assert(_end_y <= get_highest_y());
	assert(_end_x <= get_highest_x());
	for(int y = _start_y; y <= _end_y; ++y) {
		ref_levelcell_at_yx(y,_start_x).cellterrain = cellterrain;
		ref_levelcell_at_yx(y,  _end_x).cellterrain = cellterrain;
	}
	for(int x = _start_x; x <= _end_x; ++x) {
		ref_levelcell_at_yx(_start_y,x).cellterrain = cellterrain;
		ref_levelcell_at_yx(  _end_y,x).cellterrain = cellterrain;
	}
}










	void
Level::level_create_terrain_open_field(
			 int const rock_amount
			,int const rock_size_base
			,int const rock_size_dice
			,int const rubble_dice
			,unsigned randomness_seed 
			)
{
	level_add_terrain_feature_noise_with_cellterain_(
			CELLTERRAIN_RUBBLE
			,rubble_dice
			,randomness_seed
			);

	for(int rock_count = 0; rock_count < rock_amount ; ++rock_count ) {
		int const rock_size_y
			= rock_size_base
			+ (rand_r(&randomness_seed)%rock_size_dice)
			;
		int const rock_size_x
			= rock_size_base
			+ (rand_r(&randomness_seed)%rock_size_dice)
			;
		int const rock_pos_y
			= (rand_r(&randomness_seed)%get_highest_y()) ;
		int const rock_pos_x
			= (rand_r(&randomness_seed)%get_highest_x()) ;
		level_add_terrain_feature_rectangle_with_cellterain_fill(
				 CELLTERRAIN_RUBBLE
				,rock_pos_y
				,rock_pos_x
				,rock_pos_y + rock_size_y
				,rock_pos_x + rock_size_x
				);
	}
}







void
Level::level_create_terrain_messy_hall(void)
{
	level_add_terrain_feature_rectangle_with_cellterain_fill(
			CELLTERRAIN_RUBBLE
			,8
			,8
			,18
			,14
			);
	level_add_terrain_feature_rectangle_with_cellterain_fill(
			CELLTERRAIN_RUBBLE
			,8
			,8
			,10
			,30
			);
	level_add_terrain_feature_rectangle_with_cellterrain_border(
			CELLTERRAIN_HASH
			,12
			,16
			,24
			,30
			);


	level_add_terrain_feature_noise_with_cellterain_(
			 CELLTERRAIN_RUBBLE
			,8
			,0
			);


	// outer border
	level_add_terrain_feature_rectangle_with_cellterrain_border(
			CELLTERRAIN_HASH
			,0
			,0
			,get_highest_y()
			,get_highest_x()
			);

	// and empty space:
	// corridor almost in the middle
	level_add_terrain_feature_rectangle_with_cellterain_fill(
			CELLTERRAIN_NONE
			,16
			,2
			,20
			,get_highest_x() -2
			);

	// clean area around player
}




	void
Level::level_terrain_clear_around_player(void) {
	level_add_terrain_feature_rectangle_with_cellterain_fill(
			CELLTERRAIN_NONE
			,ref_player_entity().vec2d_position.y - 4
			,ref_player_entity().vec2d_position.x - 4
			,ref_player_entity().vec2d_position.y + 4
			,ref_player_entity().vec2d_position.x + 4
			);
}



















//ctor
Level::Level(
		int const _y_max 
		,int const _x_max 
		)
	: collision_table(CollisionTable(_y_max,_x_max))
{
	y_max = _y_max;
	x_max = _x_max;
	table_of_cells.resize(y_max);
	for(auto & row : table_of_cells ) {
		row.resize(x_max);
		for(auto & cell : row) {
			cell.cellterrain = CELLTERRAIN_NONE;
		}
	}



	// player entity:
	vector_of_entity.emplace_back(Entity(ID_BaseEntity_human));
	ref_player_entity().set_life_to_max();
	vector_of_entity.back().force_set_position_yx(8,8);
	//vector_of_entity.back().force_set_position_yx(get_highest_y()-3,get_highest_x()-3);

	//level_create_terrain_messy_hall();
	level_create_terrain_open_field(
			  4	// int const rock_amount
			, 3	// int const rock_size_base
			, 3	// int const rock_size_dice
			,20	// int const rubble_dice
			, 0	// unsigned randomness_seed 
			);

	level_terrain_clear_around_player();




	if(MAKE_ENEMIES) {
		// enemies:
		// shooters
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(1,2);
		// "wraith" - ignores collision with terrain
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_wraith));
		vector_of_entity.back().force_set_position_yx(30,30);
		//
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_eater));
		vector_of_entity.back().force_set_position_yx(2,5);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_devourer));
		vector_of_entity.back().force_set_position_yx(20,5);
		// a few giants
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_bengalov));
		vector_of_entity.back().force_set_position_yx(20,31);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_bengalov));
		vector_of_entity.back().force_set_position_yx(21,33);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_bengalov));
		vector_of_entity.back().force_set_position_yx(19,36);
		// 
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(19,7);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(20,4);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(20,5);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(20,6);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(19,5);
	}


}
