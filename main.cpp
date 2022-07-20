
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
	check_table_of_baseentity_is_valid();


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

	Level LEVEL  = Level(32,64);
	LEVEL.ref_player_entity().id_of_target = 1;
	LEVEL.ref_player_entity().flag_follow_target = false;


	{
		FILE * file_savefile = fopen("save.out","r");
		if(file_savefile != nullptr) {
			LEVEL.ref_player_entity().fscan_as_tsv_row(file_savefile);
			fclose(file_savefile);
		}
	}





	GlobalTimer GLOBALTIMER = GlobalTimer();
	bool DISPLAY_DEBUG = true;
	bool DEBUG_PRINT_COLLISION_TABLE = false;



	int input_character = ERR;
	int timeout_miliseconds=100;
	timeout(timeout_miliseconds);
	ESCDELAY = 10;
	// loop
	for(input_character = ERR ;  input_character != 'Z' ; input_character = getch() ) {
		mvprintw(0,0,"(press Z to quit)");
		mvprintw(1,0,"living combatants: %4d" , LEVEL.get_count_of_living_entities());
		if(LEVEL.ref_player_entity().is_dead()) {
			mvprintw(3,0,"[[RIP] - [your ded]]");
		}

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
			case '1': LEVEL.ref_player_entity().consume_food();  break;
			//
			case 'f': LEVEL.ref_player_entity().flag_follow_target = true; break;
			case 'F': LEVEL.ref_player_entity().flag_follow_target = false; break;
			  //
			case KEY_F(3): DEBUG_PRINT_COLLISION_TABLE = true; break;
			case KEY_F(4): DEBUG_PRINT_COLLISION_TABLE = false; clear(); break;
			case KEY_F(5): LEVEL.ref_player_entity().flag_stop_on_collision = true; break;
			case KEY_F(6): LEVEL.ref_player_entity().flag_stop_on_collision = false; break;
			case '\e': {
						  LEVEL.ref_player_entity().order_stop();
						  break;
			          }
			//
			case '\t': LEVEL.player_tab_target(); break;
			
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
		if(DEBUG_PRINT_COLLISION_TABLE) {
			erase();
			for(int y = 0; y < LEVEL.collision_table.size_y ; ++y ) {
				for(int x = 0; x < LEVEL.collision_table.size_x ; ++x ) {
					if(LEVEL.collision_table.is_blocked_yx(y,x)) {
						mvaddch(y,x,'x');
					}
				}
			}
			refresh();
		}
	} // loop



	endwin();

	{
		FILE * file_savefile = fopen("save.out","w");
		LEVEL.ref_player_entity().fprint_as_tsv_row(file_savefile);
		fclose(file_savefile);
	}
}
