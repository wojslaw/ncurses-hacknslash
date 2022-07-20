#pragma once
#include "Timer.hpp"


struct Ability {
	int is_selftarget = false;

	int stack_max = 3;
	int stack_current = 1;

	CountdownTimer timer_stack = CountdownTimer(20.0);

	bool is_ready(void) const { return stack_current >= 1; }

	bool consume_stack(void) {
		if(!is_ready()) {
			return false;
		}
		--stack_current;
		return true;
	}


	void update_with_seconds(double const deltatime_seconds) {
		timer_stack.update_with_deltatime_seconds(deltatime_seconds);
		if(timer_stack.is_countdown_finished()) {
			timer_stack.reset();
		}
	}
};
