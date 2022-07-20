#pragma once
#include "Timer.hpp"


struct Ability {
	bool is_ability_damage = false;
	bool is_ability_targetable = false;
	int stat_damage_base = 1;
	int stat_damage_dice = 1;
	int stat_damage_range= 1;
	int last_roll_damage = 0;

	bool is_ability_healing = false;
	int stat_heal_base = 1;
	int stat_heal_dice = 1;
	int last_roll_heal = 0;

	int stack_max = 3;
	int stack_current = 1;
	CountdownTimer timer_stack = CountdownTimer(20.0);

	unsigned randomness_seed = 0;

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

	int roll_heal(void)
	{
		bool const bool_perform_heal = consume_stack();
		if(bool_perform_heal) {
			last_roll_heal = rand_r(&randomness_seed) % (1+stat_heal_dice);
			return(stat_heal_base + last_roll_heal);
		}
		return 0;
	}

	int roll_damage(void)
	{
		last_roll_damage = rand_r(&randomness_seed) % (1+stat_damage_dice);
		return(stat_damage_base + last_roll_damage);
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
		wprintw(w
		        ,"%d /%d(%4.1f) : "
		        ,stack_current
		        ,stack_max
		        ,timer_stack.remaining_seconds
		        );
		if(is_ability_healing) {
			wprintw(w
			        ,"heal %d+d%d "
			        ,stat_heal_base
			        ,stat_heal_dice );
		} else {
			wprintw(w
			        ,"dmg %d+d%d range %d"
			        ,stat_damage_base
			        ,stat_damage_dice 
			        ,stat_damage_range );
			if(is_ability_targetable) {
				wprintw(w," (target)");
			} else {
				wprintw(w," (around)");
			}
		}
		if(is_ability_ready()) {
			wattroff(w,ATTR_ABILITY_READY);
		}
	}
};
