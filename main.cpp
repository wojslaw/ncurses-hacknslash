#include <ncurses.h>


#include <assert.h>
#include <stdlib.h>
#include <locale.h>

#include <vector>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Timer.hpp"
#include "Level.hpp"
#include "Entity.hpp"


// TODO mouse, convert to level-coordinates
















int main() {

	initscr();
	srand(time(NULL));
	raw();
	noecho();
	keypad(stdscr, TRUE);
	refresh();

	WINDOW * w_gamewindow = newwin(40,40,10,10);
	wrefresh(w_gamewindow);
	refresh();

	Level LEVEL  = Level(40,60);





	GlobalTimer GLOBALTIMER = GlobalTimer();



	int ch = ERR;
	timeout(100);
	int timeout_miliseconds=100;

	// loop
	for(ch = ERR ;  ch != 'q' ; ch = getch() ) {

		switch(ch) {
			case 'w': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_UP   ); break;
			case 'a': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_LEFT ); break;
			case 's': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_DOWN ); break;
			case 'd': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_RIGHT); break;
			case 'W': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_UP   ); break;
			case 'A': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_LEFT ); break;
			case 'S': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_DOWN ); break;
			case 'D': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_RIGHT); break;
			case ' ': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_NONE ); break;
		}

		// timers
		GLOBALTIMER.update_auto();
		LEVEL.update_time_from_globaltimer(GLOBALTIMER);


		// render text
		move(0,0);
		printw( "timeout: %d [miliseconds]\n%40f\n%40f\n"
				, timeout_miliseconds
				, GLOBALTIMER.total_seconds
				, GLOBALTIMER.deltatime_seconds
				);
		printw( "\n\n%40f\n%40f\n    \n" 
				, LEVEL.ref_player_entity().total_seconds
				, LEVEL.ref_player_entity().timer_movement.remaining_seconds
			  );
		// only render if enough time moved
		LEVEL.wprint_range(w_gamewindow,0,0,10,16);
		wrefresh(w_gamewindow);
	} // loop


	printw("\n goodbye \n");

	endwin();
}
