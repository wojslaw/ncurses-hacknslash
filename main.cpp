
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
#define WINDOW_HALFSIZE_X 10

#define WINDOW_ENTITYLIST_SIZE_Y 12
#define WINDOW_ENTITYLIST_SIZE_X 24

#define WINDOW_DETAILS_SIZE_Y 12
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

	Level LEVEL  = Level(32,32); // FIXME collision is broken on non-square maps
	LEVEL.ref_player_entity().id_of_target = 1;
	LEVEL.ref_player_entity().flag_follow_target = false;

	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_SELF_HEAL;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 8;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(8.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 5;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 4;

	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_ATTACK_AOE_SELF;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 4;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(12.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 4;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 5;

	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_ATTACK_AOE_TARGET;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 4;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(12.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 2;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 3;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_range = 2;

	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_ATTACK_TARGET;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 4;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(8.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 3;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 6;

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
	bool IS_GAME_PAUSED = false;
	double total_gameplay_seconds = 0.0;



	int input_character = ERR;
	int timeout_miliseconds=20;
	timeout(timeout_miliseconds);
	ESCDELAY = 10;
	// loop
	for(input_character = ERR ;  input_character != 'Z' ; input_character = getch() ) {
		move(0,0);
		mvprintw(getcury(stdscr)+1,0,"(press Z to quit)");
		mvprintw(getcury(stdscr)+1,0,"living: %4d (total: %4zu)" , LEVEL.get_count_of_living_entities() , LEVEL.vector_of_entity.size()-1 );
		if(LEVEL.ref_player_entity().is_dead()) {
			mvprintw(getcury(stdscr)+1,0,"[[RIP] - [your ded]]" );
			mvprintw(getcury(stdscr)+1,0,"died after %f seconds" , total_gameplay_seconds);
		}
		if(IS_GAME_PAUSED) {
			mvprintw(getcury(stdscr)+1,0,"[[PAUSED]]");
		}

		switch(input_character) {
			case '1': LEVEL.make_player_use_ability_number(1); break;
			case '2': LEVEL.make_player_use_ability_number(2); break;
			case '3': LEVEL.make_player_use_ability_number(3); break;
			case '4': LEVEL.make_player_use_ability_number(4); break;
			case '=': LEVEL.ref_player_entity().consume_food();  break;
			//
			case 'h': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_LEFT]; break;
			case 'j': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_DOWN]; break;
			case 'k': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_UP]; break;
			case 'l': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_RIGHT]; break;
			case 'H': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_LEFT ); break;
			case 'J': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_DOWN ); break;
			case 'K': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_UP ); break;
			case 'L': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_RIGHT); break;
			//
			case 'w': case KEY_UP:    LEVEL.ref_player_entity().set_direction_order(DIRECTION_UP   ); break;
			case 'a': case KEY_LEFT:  LEVEL.ref_player_entity().set_direction_order(DIRECTION_LEFT ); break;
			case 's': case KEY_DOWN:  LEVEL.ref_player_entity().set_direction_order(DIRECTION_DOWN ); break;
			case 'd': case KEY_RIGHT: LEVEL.ref_player_entity().set_direction_order(DIRECTION_RIGHT); break;
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
			case KEY_F(12): DEBUG_PRINT_COLLISION_TABLE = true; break;
			case KEY_F(11): DEBUG_PRINT_COLLISION_TABLE = false; clear(); break;
			case KEY_F(5): LEVEL.ref_player_entity().flag_stop_on_collision = true; break;
			case KEY_F(6): LEVEL.ref_player_entity().flag_stop_on_collision = false; break;
			case KEY_F(9):  IS_GAME_PAUSED = true;  break;
			case KEY_F(10): IS_GAME_PAUSED = false;  break;
			// godmode
			case KEY_F(8):
				{
					LEVEL.ref_player_entity().defense_bonus = 100;
					for(Ability & ability : LEVEL.ref_player_entity().vector_of_abilities) {
						ability.stat_roll_base = 100;
						ability.stat_range = 100;
						ability.stack_current = 100;
					}
					break;
				}
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

		if(!IS_GAME_PAUSED) {
			LEVEL.update_time_from_globaltimer(GLOBALTIMER);
			if(LEVEL.ref_player_entity().is_alive()) {
				total_gameplay_seconds += GLOBALTIMER.deltatime_seconds;
			}
		}


		// render text
		if(DISPLAY_DEBUG) {
			mvprintw(getcury(stdscr)+1,0, "timeout: %d [miliseconds]" , timeout_miliseconds);
			mvprintw(getcury(stdscr)+1,0, "%.1f" , GLOBALTIMER.total_seconds );
			mvprintw(getcury(stdscr)+1,0, "%.1f" , total_gameplay_seconds);
			mvprintw(getcury(stdscr)+1,0, "%f" , GLOBALTIMER.deltatime_seconds);
			mvprintw(getcury(stdscr)+1,0, "SFX: %4zu" , LEVEL.vector_of_visual_entity.size() );
		}
		move(0,0);
		LEVEL.wprint_render_centered_on_player_entity_fill_window(w_gamewindow);
		LEVEL.wprint_entitylist(w_text_entitylist);
		LEVEL.ref_player_entity().wprint_detailed_entity_info(w_text_player);
		werase(w_text_target);
		if( (LEVEL.ref_player_entity().has_selected_target)
		    &&
			(LEVEL.ref_player_entity().id_of_target > 0)
				) {
			LEVEL.ref_target().wprint_detailed_entity_info_enemy(w_text_target);
		}
		wrefresh(w_text_target);
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
