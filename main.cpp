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

	Level LEVEL  = Level(8,12);
	{
		LevelCell & lc = LEVEL.ref_levelcell_at_vec2d(Vec2d(2,2));
		lc.is_terrain = true;
	}





	GlobalTimer GLOBALTIMER = GlobalTimer();
	CountdownTimer ct_1 = CountdownTimer(2.0);



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
		ct_1.update_from_globaltimer(GLOBALTIMER);


		// render text
		move(0,0);
		printw( "timeout: %d [miliseconds]\n%40f\n%40f\n"
				, timeout_miliseconds
				, GLOBALTIMER.total_seconds
				, GLOBALTIMER.deltatime_seconds
				);
		printw( "\n\n%40f\n%40f\n%40d\n" 
				, ct_1.total_seconds
				, ct_1.remaining_seconds
				, ct_1.ticks_collected
			  );
		// only render if enough time moved
		LEVEL.update_table_of_cells_with_pointers_to_entities();
		LEVEL.update_entities();
		LEVEL.wprint_range(w_gamewindow,0,0,12,20);
		wrefresh(w_gamewindow);
	} // loop


	printw("\n goodbye \n");

	endwin();
}
