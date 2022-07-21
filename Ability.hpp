#pragma once
#include "Timer.hpp"

enum ABILITYTYPE {
	ABILITYTYPE_NONE = 0 ,
	ABILITYTYPE_SELF_HEAL ,
	ABILITYTYPE_ATTACK_TARGET ,
	ABILITYTYPE_ATTACK_AOE_SELF ,
	ABILITYTYPE_ATTACK_AOE_TARGET ,
};


struct Ability {
	enum ABILITYTYPE abilitytype = ABILITYTYPE_NONE;

	bool is_ability_damage(void) const {
		return(
				   (abilitytype == ABILITYTYPE_ATTACK_AOE_SELF)
				|| (abilitytype == ABILITYTYPE_ATTACK_AOE_TARGET)
				|| (abilitytype == ABILITYTYPE_ATTACK_TARGET)
				);
	}
	bool is_ability_aoe(void) const {
		return(
				(abilitytype == ABILITYTYPE_ATTACK_AOE_SELF)
				|| (abilitytype == ABILITYTYPE_ATTACK_AOE_TARGET)
				);
	}
	bool is_ability_targetable(void) const {
		return(
				  (abilitytype == ABILITYTYPE_ATTACK_AOE_TARGET)
				||(abilitytype == ABILITYTYPE_ATTACK_TARGET)
				);
	}
	bool is_ability_self_heal(void) const
	{
		return(abilitytype == ABILITYTYPE_SELF_HEAL);
	}

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
	int get_roll_min(void) const {
		return(stat_roll_base);
	}

	int get_max_roll(void) const {
		return(stat_roll_base + stat_roll_dice);
	}

	int get_damage_per_second_average(void) const {
		int const dmg_max = get_max_roll();
		int const dmg_min = stat_roll_base;
		return(((dmg_max+dmg_min)/2)/(int)timer_stack.seconds_countdown);
	}

	int get_max_total_damage(void) const {
		return(get_max_roll() * stat_range);
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
		last_roll = stat_roll_base + last_roll_of_dice;
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
		assert(abilitytype != ABILITYTYPE_NONE);
		if(is_ability_ready()) {
			wattron(w,ATTR_ABILITY_READY);
		}
		switch(abilitytype) {
			case ABILITYTYPE_NONE: wprintw(w,"[NONE]"); break;
			case ABILITYTYPE_SELF_HEAL:        wprintw(w,"heal" ); break;
			case ABILITYTYPE_ATTACK_AOE_SELF:   wprintw(w,"DMG self(AoE %d)" ,stat_range ); break;
			case ABILITYTYPE_ATTACK_AOE_TARGET: wprintw(w,"DMG targ(AoE %d)" ,stat_range ); break;
			case ABILITYTYPE_ATTACK_TARGET: wprintw(w,"DMG targ" ); break;
			default: wprintw(w,"[INVALID]");
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


};
