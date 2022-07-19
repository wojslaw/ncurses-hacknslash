
#include <ncurses.h>


#include <assert.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <vector>
#include <algorithm>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Ncurses.hpp"
#include "Timer.hpp"
#include "Level.hpp"
#include "Entity.hpp"

#define WINDOW_HALFSIZE_Y 6
#define WINDOW_HALFSIZE_X 11

#define WINDOW_ENTITYLIST_SIZE_Y 12
#define WINDOW_ENTITYLIST_SIZE_X 24

#define WINDOW_DETAILS_SIZE_Y 8
#define WINDOW_DETAILS_SIZE_X 40

#define MULTIPLY_TIME 1.0













int main()
{

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

	int const w_text_start_y
		= 2 + std::max(
				(getbegy(w_gamewindow) + getmaxy(w_gamewindow))
				,
				(getbegy(w_text_entitylist) + getmaxy(w_text_entitylist))
				);

	int const w_text_start_x
		= (COLS/2) - WINDOW_DETAILS_SIZE_X;

	// text windows
	WINDOW * w_text_player
		= newwin(
				 WINDOW_DETAILS_SIZE_Y
				,WINDOW_DETAILS_SIZE_X
				,w_text_start_y
				,w_text_start_x
				);
	WINDOW * w_text_target
		= newwin(
				 WINDOW_DETAILS_SIZE_Y
				,WINDOW_DETAILS_SIZE_X
				,w_text_start_y
				,w_text_start_x + WINDOW_DETAILS_SIZE_X
				);

	refresh();
	wrefresh(w_gamewindow);
	wrefresh(w_text_entitylist);

	box(w_text_player,0,0);
	box(w_text_target,0,0);
	wrefresh(w_text_player);
	wrefresh(w_text_target);

	Level LEVEL  = Level(64,128);
	LEVEL.ref_player_entity().id_of_target = 1;






	GlobalTimer GLOBALTIMER = GlobalTimer();
	bool DISPLAY_DEBUG = true;



	int input_character = ERR;
	int timeout_miliseconds=10;
	timeout(timeout_miliseconds);
	ESCDELAY = 10;
	int input_digit = -1;
	// loop
	for(input_character = ERR ;  input_character != 'Z' ; input_character = getch() ) {

		switch(input_character) {
			case 'x': LEVEL.make_visual_effect_on_target(5); break;
			//
			case 'h': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_LEFT]; break;
			case 'j': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_DOWN]; break;
			case 'k': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_UP]; break;
			case 'l': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_RIGHT]; break;
			//
			case 'w': LEVEL.ref_player_entity().set_direction_order(DIRECTION_UP   ); break;
			case 'a': LEVEL.ref_player_entity().set_direction_order(DIRECTION_LEFT ); break;
			case 's': LEVEL.ref_player_entity().set_direction_order(DIRECTION_DOWN ); break;
			case 'd': LEVEL.ref_player_entity().set_direction_order(DIRECTION_RIGHT); break;
			case 'c': LEVEL.ref_player_entity().set_direction_order(DIRECTION_ANGLED_DOWN_RIGHT); break;
			case 'z': LEVEL.ref_player_entity().set_direction_order(DIRECTION_ANGLED_DOWN_LEFT ); break;
			case 'e': LEVEL.ref_player_entity().set_direction_order(DIRECTION_ANGLED_UP_RIGHT  ); break;
			case 'q': LEVEL.ref_player_entity().set_direction_order(DIRECTION_ANGLED_UP_LEFT   ); break;
			//
			case 'W': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_UP   ); break;
			case 'A': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_LEFT ); break;
			case 'S': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_DOWN ); break;
			case 'D': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_RIGHT); break;
			//
			case 'f': LEVEL.ref_player_entity().flag_follow_target = true; break;
			case 'F': LEVEL.ref_player_entity().flag_follow_target = false; break;
			  //
			case '\e': {
						  LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_NONE );
						  LEVEL.ref_player_entity().reset_targeting();
						  break;
			          }
			//
			case '\t': LEVEL.player_tab_target(); break;
			
		}
		if(isdigit(input_character)) {
			input_digit = input_character - '0';
			LEVEL.player_set_target_to_visibleid_from_digit(input_digit);
		}

		// timers
		GLOBALTIMER.update_auto();
		if(MULTIPLY_TIME > 1.0) {
			GLOBALTIMER.deltatime_seconds *= MULTIPLY_TIME;
		}

		mvprintw(LINES-2,0,"update_time_from_globaltimer\n");
		LEVEL.update_time_from_globaltimer(GLOBALTIMER);


		// render text
		if(DISPLAY_DEBUG) {
			move(11,0);
			printw( "timeout: %d [miliseconds]" , timeout_miliseconds);
			move(12,0);
			printw("%f" , GLOBALTIMER.total_seconds );
			move(13,0);
			printw("%f" , GLOBALTIMER.deltatime_seconds);
			move(14,0);
			printw( "SFX: %zu" , LEVEL.vector_of_visual_entity.size() );
		}
		move(0,0);
		// only render if enough time moved
		LEVEL.wprint_centered_on_player_entity_with_window_halfsize(w_gamewindow,WINDOW_HALFSIZE_Y,WINDOW_HALFSIZE_X);
		LEVEL.wprint_entitylist(w_text_entitylist);
		LEVEL.ref_player_entity().wprint_detailed_entity_info(w_text_player);
		LEVEL.ref_target().wprint_detailed_entity_info(w_text_target);
		wrefresh(w_gamewindow);
		wrefresh(w_text_entitylist);
		//move(
		//		 getbegy(w_gamewindow)+LEVEL.ncurses_cursor_y_offset_target
		//		,getbegx(w_gamewindow)+LEVEL.ncurses_cursor_x_offset_target);
		// border around window
	} // loop


	printw("\n goodbye \n");

	endwin();
}
