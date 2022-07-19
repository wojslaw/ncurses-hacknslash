#include "Entity.hpp"


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






Entity::Entity(ID_BaseEntity const _id_of_base_entity)
	: id_base_entity(_id_of_base_entity)
{
	timer_movement = CountdownTimer(ref_base_entity().seconds_movement);
	timer_wellfed.remaining_seconds  = 20.0;
	set_life_to_max();
}





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

	if(is_ready_to_move()) {
		update_movement();
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
	wprintw(w,"%c  %2d/%2d  %s" , ncurses_get_symbol() , stat_life , get_life_max() , get_name() );
	wmove(w,2,1);
	wprintw(w,"fed:%.1f( %.1f)" ,timer_wellfed.remaining_seconds, timer_regenerate_life.remaining_seconds);
	wmove(w,3,1);
	wprintw(w,"target: %zu" , id_of_target);
	wmove(w,4,1);
	wprintw(w,"A:%d-%d , last: %d (rolled %d)"
			,get_attack_base()
			,get_attack_maximum()
			,last_combat_attack_damage
			,last_combat_attack_roll );
	wmove(w,5,1);
	wprintw(w,"@[%2d , %2d]  >[%2d , %2d] %d %d %d"
			,vec2d_position.y
			,vec2d_position.x
			,vec2d_planned_movement.y
			,vec2d_planned_movement.x
			,direction
			,direction_persistent
			,direction_persistent_ai
			);
	wmove(w,6,1);
	wprintw(w,"timer_move: %.1f"
			,timer_movement.remaining_seconds
			);
	wrefresh(w);
}








	void
Entity::set_direction_persistent(enum DIRECTION const dir)
{
	direction_persistent = dir;
	vec2d_planned_movement = DIRECTION_VECTOR[dir];
}







	void
Entity::set_direction_order(enum DIRECTION const dir) 
{
	Vec2d const & vec2d_direction_order = DIRECTION_VECTOR[dir];
	vec2d_planned_movement.add_vec2d(vec2d_direction_order);
	vec2d_planned_movement.normalize();
}











	void
Entity::position_restore_last(void) {
	vec2d_position = vec2d_position_last;
	if(flag_stop_on_collision) {
		direction = DIRECTION_NONE;
		direction_persistent = DIRECTION_NONE;
		vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_NONE];
	}
}




	void
Entity::update_movement_from_planned_movement(void)
{
	move(LINES-3,0);
	printw("update_movement_from_planned_movement %p @[%2d , %2d]  >[%2d , %2d]"
			, this
			, vec2d_position.y
			, vec2d_position.x
			, vec2d_planned_movement.y
			, vec2d_planned_movement.x
		  );
	assert(
			(vec2d_planned_movement.y != 0)
			||
			(vec2d_planned_movement.x != 0)
			);
	// 1. prepare vector of current movement
	Vec2d const vec2d_of_movement = vec2d_planned_movement.as_normalized();
	printw("m[%2d , %2d]"
			,vec2d_of_movement.y
			,vec2d_of_movement.x
			);
	// 2. perform movement
	vec2d_position.add_vec2d(vec2d_of_movement);
	// 3. modify planned direction
	vec2d_planned_movement.subtract_vec2d(vec2d_of_movement); // I had "vec2d_position" here and was wondering why movement doesn't work...
	// 4. overwrite last movement
	vec2d_last_movement = vec2d_of_movement;
}





	bool 
Entity::has_planned_movement(void)
{
	return
	        (vec2d_planned_movement.y != 0)
	         ||                            
	        (vec2d_planned_movement.x != 0);
}




	void
Entity::update_movement(void)
{
	if(is_dead()) {
		return;
	}
	// 1 save last position
	vec2d_position_last = vec2d_position;
	// and clear last movement
	vec2d_last_movement.set_zero();

	// 2. check if has persistent order
	if(direction_persistent != DIRECTION_NONE) {
		vec2d_planned_movement.add_vec2d(DIRECTION_VECTOR[direction_persistent]);
	}

	// 3. add order, if applicable
	if(direction != DIRECTION_NONE) {
		vec2d_planned_movement.add_vec2d(DIRECTION_VECTOR[direction]);
		direction = DIRECTION_NONE;
	}

	// 4. try to move from planned:
	if(has_planned_movement()) {
		update_movement_from_planned_movement();
	}

	// 5 consume a movement tick
	if(was_moved_recently()) {
		timer_movement.consume_tick();
		timer_movement.reset();
	}

}







	void
Entity::order_stop(void)
{
	set_direction_persistent(DIRECTION_NONE );
	reset_targeting();
}








	void
Entity::consume_food(void)
{
	timer_wellfed.remaining_seconds += 4.0;
}












