#include <ncurses.h>


#include <assert.h>
#include <stdlib.h>
#include <locale.h>

#include <vector>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Ncurses.hpp"
#include "Timer.hpp"
#include "Level.hpp"
#include "Entity.hpp"

#define WINDOW_HALFSIZE_Y 4
#define WINDOW_HALFSIZE_X 7

// TODO keep screen centered

// TODO mouse, convert to level-coordinates

// TODO AI: move towards player

// IDEA show enemy like in diablo - select target and it will show


// IDEA rhythm - maybe if you press the movement key at the right moment, you can move faster or something?













int main() {

	initscr();
	srand(time(NULL));
	raw();
	noecho();
	keypad(stdscr, TRUE);
	refresh();

	init_colorpairs();

	WINDOW * w_gamewindow = newwin(WINDOW_HALFSIZE_Y*2+2,WINDOW_HALFSIZE_X*2+2,10,30);
	WINDOW * w_text_entitylist = newwin(20,16,10,50);
	wrefresh(w_gamewindow);
	wrefresh(w_text_entitylist);
	refresh();

	Level LEVEL  = Level(40,60);
	LEVEL.ref_player_entity().id_of_target = 1;






	GlobalTimer GLOBALTIMER = GlobalTimer();
	bool DISPLAY_DEBUG = false;



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
		if(DISPLAY_DEBUG) {
			printw( "timeout: %d [miliseconds]\n%40f\n%40f\n"
					, timeout_miliseconds
					, GLOBALTIMER.total_seconds
					, GLOBALTIMER.deltatime_seconds
					);
			printw( "\n\n%40f\n%40f\n    \n" 
					, LEVEL.ref_player_entity().total_seconds
					, LEVEL.ref_player_entity().timer_movement.remaining_seconds
				  );
		}
		// only render if enough time moved
		border_around_window(w_gamewindow);
		border_around_window(w_text_entitylist);
		LEVEL.wprint_centered_on_player_entity_with_window_halfsize(w_gamewindow,WINDOW_HALFSIZE_Y,WINDOW_HALFSIZE_X);
		LEVEL.wprint_entitylist(w_text_entitylist,getmaxy(w_text_entitylist));
		wrefresh(w_gamewindow);
		wrefresh(w_text_entitylist);
		move(
				 getbegy(w_gamewindow)+LEVEL.ncurses_cursor_y_offset_target
				,getbegx(w_gamewindow)+LEVEL.ncurses_cursor_x_offset_target);
		// border around window
	} // loop


	printw("\n goodbye \n");

	endwin();
}
