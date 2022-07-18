#include <ncurses.h>


#include <assert.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <vector>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Ncurses.hpp"
#include "Timer.hpp"
#include "Level.hpp"
#include "Entity.hpp"

#define WINDOW_HALFSIZE_Y 5
#define WINDOW_HALFSIZE_X 8

#define WINDOW_ENTITYLIST_SIZE_Y 12
#define WINDOW_ENTITYLIST_SIZE_X 24


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

	int const w_gamewindow_size_y = WINDOW_HALFSIZE_Y*2+3; // I don't know why, but it needs +3
	int const w_gamewindow_size_x = WINDOW_HALFSIZE_X*2+3; // ah, I know: +1 player, +2 border
	int const w_gamewindow_start_y = (LINES/2)-(w_gamewindow_size_y); // I don't know why, but it needs +3 to 
	int const w_gamewindow_start_x = (COLS/2)-(w_gamewindow_size_x); // I don't know why, but it needs +3 to 

	WINDOW * w_gamewindow
		= newwin(
				 w_gamewindow_size_y
				,w_gamewindow_size_x
				,w_gamewindow_start_y
				,w_gamewindow_start_x);
	WINDOW * w_text_entitylist
		= newwin(
				 WINDOW_ENTITYLIST_SIZE_Y
				,WINDOW_ENTITYLIST_SIZE_X
				,w_gamewindow_start_y+2
				,w_gamewindow_start_x+w_gamewindow_size_x+2 );
	wrefresh(w_gamewindow);
	wrefresh(w_text_entitylist);
	refresh();

	Level LEVEL  = Level(40,60);
	LEVEL.ref_player_entity().id_of_target = 1;






	GlobalTimer GLOBALTIMER = GlobalTimer();
	bool DISPLAY_DEBUG = false;



	int input_character = ERR;
	timeout(100);
	int timeout_miliseconds=100;
	int input_digit = -1;
	// loop
	for(input_character = ERR ;  input_character != 'Z' ; input_character = getch() ) {

		switch(input_character) {
			case 'w': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_UP   ); break;
			case 'a': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_LEFT ); break;
			case 's': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_DOWN ); break;
			case 'd': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_RIGHT); break;
			case 'c': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_ANGLED_DOWN_RIGHT); break;
			case 'z': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_ANGLED_DOWN_LEFT ); break;
			case 'e': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_ANGLED_UP_RIGHT  ); break;
			case 'q': LEVEL.ref_player_entity().set_direction_temporary(DIRECTION_ANGLED_UP_LEFT   ); break;
			//
			case 'W': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_UP   ); break;
			case 'A': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_LEFT ); break;
			case 'S': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_DOWN ); break;
			case 'D': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_RIGHT); break;
			case ' ': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_NONE ); break;
			//
			case '\t': LEVEL.player_tab_target(); break;
			
		}
		if(isdigit(input_character)) {
			input_digit = input_character - '0';
			LEVEL.player_set_target_to_visibleid_from_digit(input_digit);
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
		LEVEL.wprint_centered_on_player_entity_with_window_halfsize(w_gamewindow,WINDOW_HALFSIZE_Y,WINDOW_HALFSIZE_X);
		LEVEL.wprint_entitylist(w_text_entitylist);
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
