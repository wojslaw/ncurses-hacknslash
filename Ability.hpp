#pragma once
#include "Timer.hpp"


struct Ability {
	bool is_ability_damage = false;
	bool is_ability_targetable = false;
	bool is_ability_healing = false;

	int stat_roll_base = 1;
	int stat_roll_dice = 1;
	int stat_range= 1;

	int last_roll_of_dice = 0;
	int last_roll = 0;

	bool flag_display_ability_stack_timer = false;

	int stack_max = 3;
	int stack_current = 1;
	CountdownTimer timer_stack = CountdownTimer(20.0);
	CountdownTimer timer_recently_used = CountdownTimer(2.0);

	unsigned randomness_seed = 0;

public:
	int get_max_roll(void) const {
		return(stat_roll_base + stat_roll_dice);
	}

	bool consume_stack(void) {
		if(!is_ability_ready()) {
			return false;
		}
		--stack_current;
		if(timer_stack.is_countdown_finished()) {
			timer_stack.reset();
		}
		return true;
	}

	int roll(void)
	{
		last_roll_of_dice = rand_r(&randomness_seed) % (1+stat_roll_dice);
		last_roll = stat_roll_base + last_roll;
		return(last_roll);
	}

	int roll_and_consume(void)
	{
		bool const bool_is_ready = consume_stack();
		if(bool_is_ready) {
			return(roll());
		}
		return 0;
	}


	bool is_ability_ready(void) const { return stack_current >= 1; }
	bool is_ability_full(void) const { return stack_current >= stack_max; }



	void update_time_from_globaltimer(GlobalTimer const& GLOBALTIMER)
	{
		timer_stack.update_time_from_globaltimer(GLOBALTIMER);
		if(timer_stack.is_countdown_finished()
		        &&
		        !is_ability_full()) {
			++stack_current;
			timer_stack.reset();
		}
	}


	void wprint(WINDOW * w) const
	{
		assert(w);
		assert(is_ability_healing || is_ability_damage);
		if(is_ability_ready()) {
			wattron(w,ATTR_ABILITY_READY);
		}
		if(is_ability_healing) {
			wprintw(w,"heal");
		} else {
			wprintw(w
			        ,"DMG(RNG %d)"
			        ,stat_range );
			if(is_ability_targetable) {
				wprintw(w," target");
			} else {
				wprintw(w," around");
			}
		}
		wprintw(w
				," %d-%d "
				,stat_roll_base
				,get_max_roll() );
		if(flag_display_ability_stack_timer) {
			wprintw(w
			        ,"%d/%d(%.1f) "
			        ,stack_current
			        ,stack_max
			        ,timer_stack.remaining_seconds
			        );
		}
		if(is_ability_ready()) {
			wattroff(w,ATTR_ABILITY_READY);
		}
		// also, display number of stacks:
		wattron(w,ATTR_ABILITY_STACK);
		int const start_x = (getmaxx(w)-1-stack_max);
		for(int i = 0; i < stack_current; ++i ) {
			mvwaddch(w,getcury(w),start_x+i,' ');
		}
		wattroff(w,ATTR_ABILITY_STACK);
		for(int i = stack_current; i < stack_max; ++i ) {
			mvwaddch(w,getcury(w),start_x+i,'_');
		}
	}


	void wprint_detailed(WINDOW * w) const
	{
		assert(w);
		assert(is_ability_healing || is_ability_damage);
		if(is_ability_ready()) {
			wattron(w,ATTR_ABILITY_READY);
		}
		if(is_ability_healing) {
			wprintw(w,"heal");
		} else {
			wprintw(w
			        ,"dmg(AoE %d)"
			        ,stat_range );
			if(is_ability_targetable) {
				wprintw(w," targ");
			} else {
				wprintw(w," self");
			}
		}
		wprintw(w
				,"%d-%d"
				,stat_roll_base
				,get_max_roll() );
		wprintw(w
				,"%d/%d(%.1f)"
				,stack_current
				,stack_max
				,timer_stack.remaining_seconds
			   );
		if(is_ability_ready()) {
			wattroff(w,ATTR_ABILITY_READY);
		}
		// also, display number of stacks:
		wattron(w,ATTR_ABILITY_STACK);
		int const start_x = (getmaxx(w)-1-stack_max);
		for(int i = 0; i < stack_current; ++i ) {
			mvwaddch(w,getcury(w),start_x+i,' ');
		}
		wattroff(w,ATTR_ABILITY_STACK);
		for(int i = stack_current; i < stack_max; ++i ) {
			mvwaddch(w,getcury(w),start_x+i,'_');
		}
	}
};
