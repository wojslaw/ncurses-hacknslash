#include "Entity.hpp"

#define DEBUG_SHOW_POSITION_OF_ENTITY false
#define MAX_LIFE_STEPS_TO_DISPLAY  40
#define THRESHOLD_HEAVILY_DAMAGED 4
#define DIVISOR_HEAVILY_DAMAGED_ROLL_DAMAGE 2
#define PRINT_DAMAGED_ON_LIFEBAR false









	int
Entity::fprint_as_tsv_row(FILE * f)
{
	assert(f);
	if(f==0) { return -1; }
	return fprintf(f
			,"%zu\t%d\t%d\t%d\t%d\t%lf\t%lf\n"
			,id_base_entity
			,resource_food
			,resource_money
			,explevel_level
			,explevel_points
			,total_seconds
			,timer_wellfed.remaining_seconds
		   );
}

	int
Entity::fscan_as_tsv_row(FILE * f)
{
	assert(f);
	if(f==0) { return -1; }
	return fscanf(f
			,"%zu\t%d\t%d\t%d\t%d\t%lf\t%lf\n"
			,&id_base_entity
			,&resource_food
			,&resource_money
			,&explevel_level
			,&explevel_points
			,&total_seconds
			,&timer_wellfed.remaining_seconds
		   );
}




Entity::Entity(ID_BaseEntity const _id_of_base_entity)
	: id_base_entity(_id_of_base_entity)
{
	timer_movement = CountdownTimer(ref_base_entity().seconds_movement,0.0);
	timer_regenerate_life = CountdownTimer(ref_base_entity().seconds_regen);
	set_life_to_max();
}




	bool
Entity::is_ready_to_move(void) const {
	return (timer_movement.remaining_seconds <= 0.0);
}




	bool
Entity::is_alive(void) const {
	return stat_life > 0;
}




	bool
Entity::is_dead(void) const {
	return stat_life <= 0;
}


bool
Entity::is_heavily_damaged(void) const
{
	if(!timer_recently_hit_heavily.is_countdown_finished()) {
		return true;
	}
	return (stat_life < (get_life_max()/THRESHOLD_HEAVILY_DAMAGED) );
}

	bool
Entity::is_slowed(void) const
{
	return( is_heavily_damaged() );
}




	bool
Entity::is_fully_decayed(void) const {
	return is_dead() && timer_decay.remaining_seconds <= 0;
}




	bool
Entity::is_corpse(void) const {
	return is_dead() && timer_decay.remaining_seconds > 0;
}


	bool
Entity::is_renderable(void) const
{
	if(is_alive()) {
		return true;
	}
	if(is_corpse()) {
		return true;
	}
	return false;
}












	void
Entity::revive(void)
{
	flag_skip_update = false;
	set_life_to_max();
}







	void
Entity::update_time_from_globaltimer(GlobalTimer const & GLOBALTIMER)
{
	if(flag_skip_update) {
		flag_marked_for_deletion = true;
		return;
	}

	total_seconds += GLOBALTIMER.deltatime_seconds;

	timer_combat_turn.update_time_from_globaltimer(GLOBALTIMER);
	timer_movement.update_time_from_globaltimer(GLOBALTIMER);
	timer_life.update_time_from_globaltimer(GLOBALTIMER);
	timer_recently_hit.update_time_from_globaltimer(GLOBALTIMER);
	timer_recently_hit_heavily.update_time_from_globaltimer(GLOBALTIMER);
	timer_recently_healed.update_time_from_globaltimer(GLOBALTIMER);
	timer_ability.update_time_from_globaltimer(GLOBALTIMER);
	timer_last_message.update_time_from_globaltimer(GLOBALTIMER);
	timer_is_in_battle.update_time_from_globaltimer(GLOBALTIMER);

	if(is_dead()) {
		timer_decay.update_time_from_globaltimer(GLOBALTIMER);
		if(timer_decay.is_countdown_finished()) {
			flag_skip_update = true;
		}
	}

	if(is_ready_to_move()) {
		update_movement();
	}


	// ability cooldowns
	for(Ability &ability : vector_of_abilities) {
		ability.update_time_from_globaltimer(GLOBALTIMER);
	}


	// wellfed/life regen
	timer_regenerate_life.update_time_from_globaltimer(GLOBALTIMER);
	timer_wellfed.update_time_from_globaltimer(GLOBALTIMER);
	if(timer_wellfed.remaining_seconds > 0) {
		if(timer_regenerate_life.remaining_seconds >= REGEN_SECONDS_WELLFED) {
			timer_regenerate_life.remaining_seconds = REGEN_SECONDS_WELLFED;
		}
		if(is_vampiric()) {
			timer_regenerate_life.remaining_seconds
				= std::min(REGEN_SECONDS_WELLFED
						,timer_regenerate_life.remaining_seconds
						);
		}
	}
	if(timer_regenerate_life.consume_tick()) {
		modify_life(1);
		timer_regenerate_life.reset_countdown_keep_ticks();
		if(timer_wellfed.remaining_seconds > 0) {
			timer_regenerate_life.remaining_seconds = REGEN_SECONDS_WELLFED;
		}
	}

	if(timer_recently_hit.is_countdown_finished()) {
		last_amount_of_damage_taken = 0;
	}

	if(timer_recently_healed.is_countdown_finished()) {
		last_amount_of_heal = 0;
	}

}




	void
Entity::wprint_detailed_entity_info(WINDOW * w) const
{
	assert(w);
	
	werase(w);
	box(w,0,0);
	if(DEBUG_SHOW_POSITION_OF_ENTITY) {
		wmove(w,getcury(w)+1,1);
		wprintw(w,"@[%2d , %2d]  >[%2d , %2d] %d %d %d"
				,vec2d_position.y
				,vec2d_position.x
				,vec2d_planned_movement.y
				,vec2d_planned_movement.x
				,direction
				,direction_persistent
				,direction_persistent_ai
				);
	}

	{ // life
		wmove(w,getcury(w)+1,1);
		int const attr = ncurses_get_attr_life();
		if(attr) {
			wattron(w,attr);
		}
		wprintw(w,"%2d/%2d"
				, stat_life
				, get_life_max()
			   );
		wprint_lifebar_at_the_rightmost(w);
	} //life

	wmove(w,getcury(w)+1,1);
	wprintw(w,"%c"
			, ncurses_get_symbol()
			);
	wprintw(w," %s  lvl %d (%d /%d)"
			, get_name()
			, explevel_level
			, explevel_points
			, explevel_points_for_next_level()
			);






	/* wmove(w,3,1); */
	/* wprintw(w,"target: %zu" , id_of_target); */
	wmove(w,getcury(w)+1,1);
	wprintw(w,"fed:%.1f( %.1f)" ,timer_wellfed.remaining_seconds, timer_regenerate_life.remaining_seconds);
	wmove(w,getcury(w)+1,1);
	wprintw(w,"DEF %d  ATK:%d-%d  range %d"
			,get_defense()
			,get_attack_base()
			,get_attack_maximum() 
			,combat_get_range()) ;
	wmove(w,getcury(w)+1,1);
	if(timer_is_in_battle.remaining_seconds > 0) {
		wprintw(w," last %d, roll %d"
				,last_combat_attack_damage
				,last_combat_attack_roll );
		if(!timer_movement.is_countdown_finished()) {
			wprintw(w," (moved)");
		}
	}
	wmove(w,getcury(w)+1,1);
	if(timer_ability.remaining_seconds > 0) {
		wprintw(w,"cooldown: %.1f"
				,timer_ability.remaining_seconds
				);
	}

	{ // abilities 
	int ability_number = 1;
	for(Ability const& ability : vector_of_abilities ) {
		wmove(w,getcury(w)+1,1);
		wprintw(w,"a%d " , ability_number);
		ability.wprint(w);
		++ability_number;
	}
	}//abilities 

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
	assert(
			(vec2d_planned_movement.y != 0)
			||
			(vec2d_planned_movement.x != 0)
			);
	// 1. prepare vector of current movement
	Vec2d const vec2d_of_movement = vec2d_planned_movement.as_normalized();
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
	if(is_recently_moved()) {
		timer_movement.consume_tick();
		timer_movement.reset();
		timer_movement.remaining_seconds *= (double)vec2d_last_movement.get_length_taxicab();
		if(is_slowed()) { // even more penalty to low-healthers
			timer_movement.remaining_seconds *= 1.5;
		}
	}

}





	void
Entity::order_stop_only_movement(void)
{
	set_direction_persistent(DIRECTION_NONE );
	vec2d_planned_movement.set_zero();
}



	void
Entity::order_stop_full_stop(void)
{
	order_stop_only_movement();
	reset_targeting();
}








	void
Entity::consume_food(void)
{
	if(resource_food >= 1) {
		timer_wellfed.remaining_seconds += 4.0;
		--resource_food;
	}
}






	void
Entity::reset_targeting(void)
{
	id_of_target = 0 ;
	has_selected_target = false;
}



	void
Entity::set_target_to_entityid(size_t const entityid)
{ 
	id_of_target = entityid ; 
	has_selected_target = true;
}




bool
Entity::is_ready_to_attack(void) const
{
	if(is_dead()) {
		return 0;
	}
	if(timer_combat_turn.remaining_seconds > 0.0) {
		return 0;
	}
	if(is_recently_moved()) {
		return false;
	}
	return timer_combat_turn.remaining_seconds <= 0;
}



char const *
Entity::get_name(void) const
{
	return ref_base_entity().name;
}

int
Entity::ncurses_get_attrs(void) const
{
	int attr = 0;
	if(is_heavily_damaged()) {
		attr = attr | ATTR_HEAVILY_DAMAGED;
	}
	if(timer_recently_hit.remaining_seconds > 0.0) {
		attr = attr | ATTR_RECENTLY_HIT;
	}
	return attr;
}


	int
Entity::ncurses_get_attr_life(void) const
{
	if(!timer_recently_healed.is_countdown_finished()) {
		if(last_amount_of_heal > last_amount_of_damage_taken) {
			return(ATTR_RECENTLY_HEALED);
		}
	}
	if(is_heavily_damaged()) {
		return(ATTR_HEAVILY_DAMAGED);
	}
	if(!timer_recently_hit.is_countdown_finished()) {
		return(ATTR_RECENTLY_HIT);
	}

	return(0);
}

	int
Entity::ncurses_get_attr_lifebar(void) const
{
	return(WA_REVERSE|ncurses_get_attr_life());
}




	bool
Entity::is_blocking(void) const
{ 
	if(!has_collision()) {
		return false;
	}
	if(is_dead()) {
		return false;
	}
	return true;
}



















	int
Entity::combat_get_range(void) const
{
	return ref_base_entity().attack_range;
}




	int
Entity::combat_roll_damage(void)
{
	if(!is_ready_to_attack()) {
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




	int
Entity::combat_roll_damage_against_defense(int const defense)
{
	timer_is_in_battle.reset();
	last_combat_attack_damage = combat_roll_damage() - defense;
	if(last_combat_attack_damage <= 0) {
		return 0;
	}
	if( is_vampiric() ) {
		timer_wellfed.remaining_seconds += (double)last_combat_attack_damage;
	}
	return last_combat_attack_damage;
}




	void
Entity::take_damage(int const damage_to_take)
{
	timer_is_in_battle.reset();
	if(damage_to_take <= 0) {
		return;
	}
	stat_life -= damage_to_take;
	timer_recently_hit.reset();
	if(damage_to_take >= (stat_life)/THRESHOLD_HEAVILY_DAMAGED) {
		timer_recently_hit_heavily.reset();
	}
}




	void
Entity::modify_life(int const delta_life)
{
	if(is_dead()) {
		return;
	}
	if(delta_life == 0) {
		return;
	}

	stat_life += delta_life;
	if(stat_life > get_life_max()) {
		stat_life = get_life_max();
	}
	if(delta_life > 0) {
		timer_recently_healed.reset();
		last_amount_of_heal = delta_life;
	}
	if(delta_life < 0) {
		timer_recently_hit.reset();
		last_amount_of_damage_taken = (-delta_life);
	}

}







	int
Entity::wprint_with_additional_attrs(
		 WINDOW * w
		,int const attrs_additional
		) const {
	assert(w);
	if(!is_renderable()) {
		return ERR;
	}
	// only render corpses if nothing is written in the character
	if(is_dead()) {
		if((winch(w) & A_CHARTEXT) != ' ') {
			return ERR;
		}
	}
	int const attrs = ncurses_get_attrs() | attrs_additional;
	int const symbol = ncurses_get_symbol();
	if(attrs != 0) { wattron(w, attrs);}
	waddch(w,symbol);
	if(attrs != 0) { wattroff(w,attrs);}
	return OK;
}







	int
Entity::ncurses_get_symbol(void) const {
	if(is_dead()) {
		if(timer_decay.remaining_seconds > 0) {
			return '%';
		}
		return 0;
	}
	return ref_base_entity().ncurses_symbol;
}








	void
Entity::take_damage_as_fraction_of_current(
		 int const multiply
		,int const divide)
{
	take_damage((stat_life*multiply)/divide);
}




	void
Entity::take_damage_as_fraction_of_max(
		 int const multiply
		,int const divide)
{
	take_damage((get_life_max()*multiply)/divide);
}






	bool
Entity::is_ready_to_cast_ability(void) const
{
	if(is_dead()) {
		return false;
	}
	return timer_ability.is_countdown_finished();
}









	void
Entity::wprint_entitylist_row(WINDOW * w,bool const is_target) const
{
	assert(w);
	int const attr
		= is_target
		? ATTR_TARGET
		: 0;
	wprint_with_additional_attrs(w,attr);
	wprintw(w
			,"%2d/%2d"
			,stat_life
			,get_life_max()
		   );

	wprint_lifebar_at_the_rightmost(w);
}


	void
Entity::wprint_lifebar_at_the_rightmost(
		 WINDOW * w
	) const
{
	assert(w);

	int const length_of_lifebar
		= std::min(
				MAX_LIFE_STEPS_TO_DISPLAY
				,getmaxx(w)-8
				);

	wmove(w,getcury(w),getmaxx(w)-length_of_lifebar-1);
	int const attr = ncurses_get_attr_lifebar();
	wattron(w,attr);
	int const life_steps_to_display
		= (stat_life
				*
				length_of_lifebar)
		/ get_life_max();
	if(is_heavily_damaged()) {
		for(int i = 0; i < life_steps_to_display; ++i) {
			waddch(w,'+');
		}
	} else {
		for(int i = 0; i < life_steps_to_display; ++i) {
			waddch(w,'+');
		}
	}
	wattroff(w,WA_REVERSE);
	if(PRINT_DAMAGED_ON_LIFEBAR) {
		if(is_heavily_damaged()) {
			if(length_of_lifebar >= 8) {
				wmove(w,getcury(w),(getmaxx(w)-length_of_lifebar));
				wprintw(w,"DAMAGED");
			}
		}
	}
	wattroff(w,attr);
}







	void
Entity::wprint_detailed_entity_info_enemy(WINDOW * w) const
{
	assert(w);

	werase(w);
	box(w,0,0);

	{ // life
		wmove(w,getcury(w)+1,1);
		int const attr = ncurses_get_attr_life();
		if(attr) {
			wattron(w,attr);
		}
		wprintw(w,"%2d/%2d"
				, stat_life
				, get_life_max()
			   );
		wprint_lifebar_at_the_rightmost(w);
	} //life

	wmove(w,getcury(w)+1,1);
	wprint_with_additional_attrs(w,0);
	wmove(w,getcury(w)+1,3);
	wprintw(w,"%s"
			, get_name()
			);

	/* wmove(w,3,1); */
	/* wprintw(w,"target: %zu" , id_of_target); */
	wmove(w,getcury(w)+1,1);
	wprintw(w,"ATK:%d-%d , range: %d"
			,get_attack_base()
			,get_attack_maximum()
			,combat_get_range());
	wmove(w,getcury(w)+1,1);
	wprintw(w,"DEF:%d"
			,get_defense() );

	if(!has_collision()) {
		wmove(w,getcury(w)+1,1);
		wprintw(w,"ghost (no collision)");
	}

	if(is_vampiric()) {
		wmove(w,getcury(w)+1,1);
		wprintw(w,"vampiric (regens dealing damage)");
	}

	if(has_destroyer_of_terrain()) {
		wmove(w,getcury(w)+1,1);
		wprintw(w,"destroyer (destroys obstacles)");
	}

	wrefresh(w);
}






