// arena-ncurses-of-famewar

#include <ncurses.h>


#include <assert.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <limits.h>
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

#define WINDOW_DETAILS_SIZE_Y 14
#define WINDOW_DETAILS_SIZE_X 40

#define MULTIPLY_TIME 1.0
#define SECONDS_MINIMUM_TIME_BEFORE_UPDATE (1.0/100.0)


#define FLAG_MOUSE_SCROLL_TARGETS true





#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>












int main()
{

	check_table_of_baseentity_is_valid();


	initscr();
	srand(time(NULL));
	raw();
	noecho();
	keypad(stdscr, TRUE);
	refresh();

	mousemask(
         BUTTON_SHIFT            
       | BUTTON_CTRL             
       | BUTTON_ALT              
       | ALL_MOUSE_EVENTS        
       | REPORT_MOUSE_POSITION   
	   , 0);
	MEVENT mevent = {0,0,0,0,0};
	mouseinterval(0);

	init_colorpairs();

	int const w_gamewindow_size_y = WINDOW_HALFSIZE_Y*2+1; // I don't know why, but it needs +3
	int const w_gamewindow_size_x = WINDOW_HALFSIZE_X*2+1; // ah, I know: +1 player, +2 border
	int const w_gamewindow_start_y = (LINES/2)-(w_gamewindow_size_y);
	int const w_gamewindow_start_x = (COLS/2)-(w_gamewindow_size_x);

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

	//box(w_text_player,0,0);
	//box(w_text_target,0,0);
	wrefresh(w_text_player);
	wrefresh(w_text_target);

	Level LEVEL  = Level(80,40);
	LEVEL.ref_player_entity().id_of_target = 1;
	LEVEL.ref_player_entity().flag_follow_target = false;

	//// explosion
	//LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	//LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_ATTACK_AOE_TARGET;
	//LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 8;
	//LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(3.0);
	//LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 1;
	//LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 2;

	// sniper shot : should add weapon damage
	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_ATTACK_AOE_TARGET;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 3;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(6.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 4;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 6;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_range = 0;

	// hopeful spark
	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_SELF_HEAL;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 8;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(8.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 5;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 4;

	// 
	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_ATTACK_AOE_SELF;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 4;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(20.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 2;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 5;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_range = 3;

	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_ATTACK_AOE_TARGET;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 4;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(12.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 2;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 3;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_range = 1;

	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_ATTACK_TARGET;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 4;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(8.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = 3;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice = 6;

	// Blood divination:
	LEVEL.ref_player_entity().vector_of_abilities.emplace_back(Ability());
	LEVEL.ref_player_entity().vector_of_abilities.back().abilitytype = ABILITYTYPE_SELF_HEAL;
	LEVEL.ref_player_entity().vector_of_abilities.back().stack_max = 1;
	LEVEL.ref_player_entity().vector_of_abilities.back().timer_stack = CountdownTimer(4.0);
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_base = -2;
	LEVEL.ref_player_entity().vector_of_abilities.back().stat_roll_dice =  1;

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
			case '1': LEVEL.make_player_use_ability_id_autotarget(1); break;
			case '2': LEVEL.make_player_use_ability_id_autotarget(2); break;
			case '3': LEVEL.make_player_use_ability_id_autotarget(3); break;
			case '4': LEVEL.make_player_use_ability_id_autotarget(4); break;
			case '5': LEVEL.make_player_use_ability_id_autotarget(5); break;
			// movement 
			case '\n':
			case '\r':
				{
					LEVEL.ref_player_entity().order_stop_only_movement();
					break;
				}
			// vim
			case 'h': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_LEFT]; break;
			case 'j': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_DOWN]; break;
			case 'k': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_UP]; break;
			case 'l': LEVEL.ref_player_entity().vec2d_planned_movement = DIRECTION_VECTOR[DIRECTION_RIGHT]; break;
			case 'H': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_LEFT ); break;
			case 'J': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_DOWN ); break;
			case 'K': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_UP ); break;
			case 'L': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_RIGHT); break;
			// wasd+diagonals
			case 'w': case KEY_UP:    LEVEL.ref_player_entity().set_direction_order(DIRECTION_UP   ); break;
			case 'a': case KEY_LEFT:  LEVEL.ref_player_entity().set_direction_order(DIRECTION_LEFT ); break;
			case 's': case KEY_DOWN:  LEVEL.ref_player_entity().set_direction_order(DIRECTION_DOWN ); break;
			case 'd': case KEY_RIGHT: LEVEL.ref_player_entity().set_direction_order(DIRECTION_RIGHT); break;
			case 'c': LEVEL.ref_player_entity().set_direction_order(DIRECTION_ANGLED_DOWN_RIGHT); break;
			case 'z': LEVEL.ref_player_entity().set_direction_order(DIRECTION_ANGLED_DOWN_LEFT ); break;
			case 'e': LEVEL.ref_player_entity().set_direction_order(DIRECTION_ANGLED_UP_RIGHT  ); break;
			case 'q': LEVEL.ref_player_entity().set_direction_order(DIRECTION_ANGLED_UP_LEFT   ); break;
			case 'W': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_UP   ); break;
			case 'A': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_LEFT ); break;
			case 'S': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_DOWN ); break;
			case 'D': LEVEL.ref_player_entity().set_direction_persistent(DIRECTION_RIGHT); break;
			//
			case 'f': LEVEL.ref_player_entity().flag_follow_target = true; break;
			case 'F': LEVEL.ref_player_entity().flag_follow_target = false; break;
			  //
			case KEY_F(1):  IS_GAME_PAUSED = true;  break;
			case KEY_F(2): IS_GAME_PAUSED = false;  break;
			// ghost
			case KEY_F(5): LEVEL.ref_player_entity().flag_stop_on_collision = true; break;
			case KEY_F(6): LEVEL.ref_player_entity().flag_stop_on_collision = false; break;
			// spawn a lot of garbage
			case KEY_F(7):
				{
					for(int i = 0; i < 100; ++i) {
						LEVEL.create_random_enemy_group();
					}
					break;
				}
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
			// godmode
			case KEY_F(9):
				{
					LEVEL.roll_new_random_feature();
					break;
				}
			//
			case KEY_F(11): DEBUG_PRINT_COLLISION_TABLE = false; clear(); break;
			case KEY_F(12): DEBUG_PRINT_COLLISION_TABLE = true; break;

			case '\e': {
						  LEVEL.ref_player_entity().order_stop_full_stop();
						  break;
			          }

			//tab target
			case KEY_DC:
				{
					LEVEL.ref_player_entity().reset_targeting();
					break;
				}
			case KEY_NPAGE: // pagedown
			case '\t':      // tab
				{
					LEVEL.player_tab_target(+1);
					break;
				}
			case KEY_PPAGE: // pagedown
			case KEY_BTAB:  // shift+tab
				{
					LEVEL.player_tab_target(-1);
					break;
				}
			case KEY_HOME:  // 
				{
					LEVEL.player_set_target_to_visibleid(1);
					break;
				}
			case KEY_END:  // 
				{
					LEVEL.player_set_target_to_visibleid(INT_MAX);
					break;
				}
			
		}

		//mouse
		getmouse(&mevent);

		//	trafo
		int mouse_w_gamewindow_y = mevent.y;
		int mouse_w_gamewindow_x = mevent.x;
		int mouse_w_entitylist_y = mevent.y;
		int mouse_w_entitylist_x = mevent.x; ++mouse_w_entitylist_x;

		bool const trafo_mouse_w_gamewindow = wmouse_trafo(
				 w_gamewindow // const WINDOW* win,
				,&mouse_w_gamewindow_y // int* pY,
				,&mouse_w_gamewindow_x // int* pX,
				,false // bool to_screen);
			 );
		bool const trafo_mouse_w_entitylist = wmouse_trafo(
				 w_text_entitylist // const WINDOW* win,
				,&mouse_w_gamewindow_y // int* pY,
				,&mouse_w_gamewindow_x // int* pX,
				,false // bool to_screen);
			 );


		// send to level
		if(trafo_mouse_w_gamewindow) {
			LEVEL.handle_input_mouse(
					w_gamewindow
					,mouse_w_gamewindow_y// int x
					,mouse_w_gamewindow_x// int y
					,mevent.bstate // mmask_t bstate
					);
		}
		if(trafo_mouse_w_entitylist) {
			switch(mevent.bstate) {
				case BUTTON1_CLICKED:
				case BUTTON1_PRESSED:
					{
						LEVEL.player_set_target_to_visibleid(
								(mouse_w_entitylist_y - 3) // don't know why, but 3 works correctly - I thought it should be 2
								// +1 border
								// +1 player(id 0)
								// +1 - where from?
								);
					} break;
			}
		}
		// scroll targets with mouse
		if(FLAG_MOUSE_SCROLL_TARGETS) {
			if(mevent.bstate == BUTTON4_PRESSED) { // scrollup
				LEVEL.player_tab_target(-1);
			}
			if(mevent.bstate == BUTTON5_PRESSED) { // scrolldown
				LEVEL.player_tab_target(+1);
			}
			if(mevent.bstate == BUTTON2_PRESSED) { // middleclick
				LEVEL.ref_player_entity().reset_targeting();
			}
		}

		//end mouse


		// timers

		if(!IS_GAME_PAUSED) {
			if(GLOBALTIMER.deltatime_seconds >= SECONDS_MINIMUM_TIME_BEFORE_UPDATE) {
				LEVEL.update_time_from_globaltimer(GLOBALTIMER);
				if(LEVEL.ref_player_entity().is_alive()) {
					total_gameplay_seconds += GLOBALTIMER.deltatime_seconds;
				}
				GLOBALTIMER.reset();
			}
			GLOBALTIMER.update_with_multiplier(MULTIPLY_TIME);
		}


		// render text
		if(DISPLAY_DEBUG) {
			mvprintw(getcury(stdscr)+1,0, "timeout: %d [miliseconds]" , timeout_miliseconds);
			mvprintw(getcury(stdscr)+1,0, "%.1f" , GLOBALTIMER.total_seconds );
			mvprintw(getcury(stdscr)+1,0, "%.1f" , total_gameplay_seconds);
			mvprintw(getcury(stdscr)+1,0, "%f" , GLOBALTIMER.deltatime_seconds);
			mvprintw(getcury(stdscr)+1,0, "SFX: %4zu" , LEVEL.vector_of_visual_entity.size() );
		}
		LEVEL.wrender_level_centered_on_player_entity_fill_window(w_gamewindow);
		LEVEL.wprint_entitylist(w_text_entitylist);
		LEVEL.ref_player_entity().wprint_detailed_entity_info(w_text_player);
		werase(w_text_target);
		if( LEVEL.ref_player_entity().has_selected_target) {
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
