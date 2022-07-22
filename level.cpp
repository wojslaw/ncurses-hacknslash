#include "Level.hpp"

#define SQUISH_FEATURES_WHEN_TOO_BIG true

#define MAKE_ENEMIES true
#define FLAG_SKIP_FIRST_ENTITY_IN_LIST_OF_VISIBLE  true

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
	// TODO errorcheck
	assert(y >= 0);
	assert(x >= 0);
	assert(y <= get_highest_y());
	assert(x <= get_highest_x());
	assert(is_position_within_bounds_of_level_yx(y,x));
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
	assert(v.y <= get_highest_y());
	assert(v.x <= get_highest_x());
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
	if(v.y > get_highest_y()) {
		return false;
	}
	if(v.x > get_highest_x()) {
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
			if(is_vec2d_position_within_rectangle(
						 visual_entity_rendered.vec2d_position
						,pos_inlevel_start_y
						,pos_inlevel_start_x
						,pos_inlevel_last_y
						,pos_inlevel_last_x
						)) {
				int const pos_window_y = visual_entity_rendered.vec2d_position.y - pos_inlevel_start_y;
				int const pos_window_x = visual_entity_rendered.vec2d_position.x - pos_inlevel_start_x;
				wmove(w,pos_window_y,pos_window_x);
				visual_entity_rendered.wprint(w);
			}
		}
	} //SFX

	{ // 4 entities
		for(Entity const& entity_rendered : vector_of_entity ) {
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
				entity_rendered.wprint_with_additional_attrs(w,0);
			}
		}
	} //entities


	{ // ensure the target is always displayed
		Entity const& entity_rendered = ref_target();
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
			entity_rendered.wprint_with_additional_attrs(w,ATTR_TARGET);
		}
	}//ensure the target is always displayed


	// TODO decouple calculations from rendering
	update_vector_of_entityids_with_entities_within_rectangle(
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
Level::make_player_use_ability_number(int const number)
{
	if(not ref_player_entity().is_ready_to_cast_ability()) {
		// TODO notify not ready
		return;
	}
	assert(number >= 1);
	size_t const id_ability = number-1;
	assert(id_ability < ref_player_entity().vector_of_abilities.size());
	Ability& ref_ability = ref_player_entity().vector_of_abilities.at(id_ability);
	if(!ref_ability.is_ability_ready()) {
		// TODO notify not ready
		return;
	}

	if(     (not ref_player_entity().has_selected_target)
	        &&
	        (ref_ability.abilitytype == ABILITYTYPE_ATTACK_TARGET )
	        ) {
		return;
	}

	std::vector<IDEntity> vector_identity_of_picked_targets;

	switch(ref_ability.abilitytype) {
		case ABILITYTYPE_SELF_HEAL:
			{
				ref_player_entity().make_entity_use_healing_ability_id(id_ability);
				break;
			}
		case ABILITYTYPE_ATTACK_AOE_TARGET:
			{
				make_visual_effect_on_target(ref_ability.stat_range);
				vector_identity_of_picked_targets
					= get_targetable_entities_around_point_with_range_skip_player(
							ref_target().vec2d_position
							,ref_ability.stat_range
							);
				break;
			}
		case ABILITYTYPE_ATTACK_AOE_SELF:
			{
				make_visual_effect_on_player(ref_ability.stat_range);
				vector_identity_of_picked_targets
					= get_targetable_entities_around_point_with_range_skip_player(
							ref_player_entity().vec2d_position
							,ref_ability.stat_range
							);
				break;
			}
		case ABILITYTYPE_ATTACK_TARGET:
			{
				make_visual_effect_on_target(0);
				vector_identity_of_picked_targets.emplace_back(ref_player_entity().id_of_target);
				break;
			}
		default:
			{
				move(0,0);
				printw("invalid ability type %d" , ref_ability.abilitytype);
				fprintf(stderr,"invalid ability type %d", ref_ability.abilitytype);
			}
	}

	if(vector_identity_of_picked_targets.size() > 0) {
		int const damage_to_deal = ref_player_entity().make_entity_roll_ability_id(id_ability);
		for(IDEntity identity : vector_identity_of_picked_targets ) {
			Entity & ref_entity = ref_from_entityid(identity);
			ref_entity.take_damage(damage_to_deal);
		}
	}
}



	void
Level::update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER)
{
	total_seconds += GLOBALTIMER.deltatime_seconds;

	timer_create_new_enemy.update_time_from_globaltimer(GLOBALTIMER);
	if(timer_create_new_enemy.is_countdown_finished()) {
		timer_create_new_enemy.reset();
		create_random_enemy_group();
	}

	{// visual entities
		bool is_any_visual_entity_valid = false;
		for(VisualEntity & visual_entity : vector_of_visual_entity) {
			visual_entity.update_time_from_globaltimer(GLOBALTIMER);
			if(visual_entity.is_valid()) {
				is_any_visual_entity_valid = true;
			}
		}
		if(!is_any_visual_entity_valid) {
			vector_of_visual_entity.clear();
		}
	}

	// entities
	for(Entity & entity : vector_of_entity) {
		entity.update_time_from_globaltimer(GLOBALTIMER);
	}
	// 
	delete_decayed_entities_if_player_has_no_target();
	//
	ensure_entities_are_within_bounds();
	update_table_of_cells_with_pointers_to_entities();
	update_collision_table();
	//
	update_entities_direction_planned();
	update_entities_positions();
	update_entity_combat_rounds();
}






	void
Level::wprint_entitylist(
		WINDOW * w)
{
	assert(w);
	werase(w);
	box(w,0,0);
	wmove(w,0,0);
	int const max_entities_to_print = getmaxy(w)-(2+1); // +2 for borders, +1
	int count_of_printed = 0;
	for(size_t id : vector_of_entityids_on_screen) {
		if(FLAG_SKIP_FIRST_ENTITY_IN_LIST_OF_VISIBLE) {
			if(id == 0) {
				continue;
			}
		}
		if(count_of_printed >= max_entities_to_print) {
			break;
		}
		//
		bool const is_target = id == ref_player_entity().id_of_target;
		Entity const& entity = vector_of_entity.at(id);
		wmove(w,getcury(w)+1,1);
		entity.wprint_entitylist_row(w,is_target);
		++count_of_printed;
	}

	if(vector_of_entityids_on_screen.size() > 1) {
		wmove(w,getcury(w)+1,1);
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
Level::player_tab_target(int const delta)
{
	assert(delta != 0);
	Entity & player_entity = ref_player_entity();
	if(vector_of_entityids_on_screen.size() <= 1 ) {
		player_entity.reset_targeting();
		return;
	}

	// find visibleid for entityid of target
	size_t const visibleid_of_target = visibleid_from_entityid(player_entity.id_of_target);
	size_t const visibleid_of_next_target = visibleid_of_target + 1;
	size_t const visibleid_of_prev_target = visibleid_of_target - 1;
	size_t const highest_visibleid = vector_of_entityids_on_screen.size() - 1;
	size_t const lowest_visibleid  = 1;

	// set visible id
	if(delta >= +1) {
		if(visibleid_of_next_target <= highest_visibleid) {
			player_entity.set_target_to_entityid(entityid_from_visibleid(visibleid_of_next_target));
		} else {
			player_entity.set_target_to_entityid(entityid_from_visibleid(lowest_visibleid)); // lowest expected visibleid
		}
	}

	if(delta <= -1) {
		if(visibleid_of_next_target > lowest_visibleid) {
			player_entity.set_target_to_entityid(entityid_from_visibleid(visibleid_of_prev_target));
		} else {
			player_entity.set_target_to_entityid(entityid_from_visibleid(highest_visibleid)); // lowest expected visibleid
		}
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
		if(distance > attacker.combat_get_range()) {
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
	assert(range >= 0);
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
	assert(range >= 0);
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
Level::delete_decayed_entities_if_player_has_no_target(void)
{
	if(ref_player_entity().has_selected_target) {
		return;
	}

	if(vector_of_entity.size() < 2) {
		return;
	}

	// go from end to start
	for(size_t i = vector_of_entity.size()-1; i > 1; --i ) {
		Entity const& entity = vector_of_entity.at(i);
		if(entity.flag_marked_for_deletion) {
			vector_of_entity.erase(vector_of_entity.begin() + i);
		}
	}
}





	void
Level::ensure_entities_are_within_bounds(void)
{
	for(Entity & ref_entity : vector_of_entity ) {
		//ref_entity.update_position();
		ref_entity.vec2d_position.y = std::max(ref_entity.vec2d_position.y,0);
	    ref_entity.vec2d_position.x = std::max(ref_entity.vec2d_position.x,0);
		ref_entity.vec2d_position.y = std::min(ref_entity.vec2d_position.y,get_highest_y());
		ref_entity.vec2d_position.x = std::min(ref_entity.vec2d_position.x,get_highest_x());
	}
}
















	void
Level::update_vector_of_entityids_with_entities_within_rectangle(
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
Level::update_vector_of_entityids_with_entities_within_range_of_player(int const range_of_search)
{
	vector_of_entityids_on_screen.resize(0);
	bool is_target_on_screen = false;
	for(size_t id = 0; id < vector_of_entity.size(); ++id) {
		Entity const& entity = vector_of_entity.at(id);
		if(entity.is_dead()) { //don't show corpses
			continue;
		}
		//
		int const distance
			= vec2d_highest_distance_between(
					 ref_player_entity().vec2d_position
					,entity.vec2d_position
					);
		if(distance <= range_of_search) {
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








	void
Level::create_random_enemy_group(void)
{
	int const enemy_group_type = rand_r(&seed)%5;
	// pick some random empty spot
	// TODO pick an empty cell
	int const y = rand_r(&seed)%get_highest_y();
	int const x = rand_r(&seed)%get_highest_x();

	if(enemy_group_type == 0) { // bengalovie
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_bengalov));
		vector_of_entity.back().force_set_position_yx(y,x);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_bengalov));
		vector_of_entity.back().force_set_position_yx(y-2,x+2);
	}

	if(enemy_group_type == 1) { // dickevs and gthrowers and eater
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(y,x);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(y-2,x+2);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(y-4,x+2);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(y-2,x+4);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_eater));
		vector_of_entity.back().force_set_position_yx(y-4,x+4);
	}

	if(enemy_group_type == 2) { // wraiths
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_wraith));
		vector_of_entity.back().force_set_position_yx(y,x);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_wraith));
		vector_of_entity.back().force_set_position_yx(y-1,x-1);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_wraith));
		vector_of_entity.back().force_set_position_yx(y+2,x+2);
	}

	if(enemy_group_type == 3) { // more dickevs and gthrowers
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(y,x);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(y,x+1);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(y,x+2);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_dickev));
		vector_of_entity.back().force_set_position_yx(y,x+3);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(y-1,x);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(y-2,x);
	}

	if(enemy_group_type == 4) { // orcs and gthrowers
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_orc));
		vector_of_entity.back().force_set_position_yx(y,x);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_orc));
		vector_of_entity.back().force_set_position_yx(y,x+1);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_orc));
		vector_of_entity.back().force_set_position_yx(y,x+3);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(y-1,x);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(y-2,x);
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_gthrower));
		vector_of_entity.back().force_set_position_yx(y-2,x+2);
	}
}




	std::vector<IDEntity> 
Level::get_targetable_entities_around_point_with_range_skip_player(
		Vec2d const& point
		,int const range
		) const
{
	assert(range >= 0);
	assert(vector_of_entity.size() > 0);
	std::vector<IDEntity> vec;
	for(IDEntity id = 1; id < vector_of_entity.size(); ++id) {
		Entity const& entity = vector_of_entity.at(id);
		int const distance 
			= vec2d_highest_distance_between(
					point
					,entity.vec2d_position
					);
		if(distance <= range) {
			vec.push_back(id);
		}
	}
	return(vec);
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
			  8	// int const rock_amount
			, 4	// int const rock_size_base
			, 2	// int const rock_size_dice
			,40	// int const rubble_dice
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
		vector_of_entity.emplace_back(Entity(ID_BaseEntity_wraith));
		vector_of_entity.back().force_set_position_yx(28,30);
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
		// 
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

















	void
Level::update_entities_positions(void) {

	//
	for(Entity & ref_entity : vector_of_entity ) {
		//if(!(is_vec2d_position_within_bounds_of_level(ref_entity.vec2d_position))) { continue;  } // skip invalidly-placed
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
	//
	//
}
