#include <ncurses.h>


#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <locale.h>

#include <vector>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Timer.h"
















int main() {

	initscr();
	srand(time(NULL));
	raw();
	noecho();
	keypad(stdscr, TRUE);
	refresh();






	GlobalTimer GLOBALTIMER = GlobalTimer();
	CountdownTimer ct_1 = CountdownTimer(2.0);



	int ch = ERR;
	timeout(100);
	int timeout_miliseconds=100;

	// loop
	for(ch = ERR ;  ch != 'q' ; ch = getch() ) {

		switch(ch) {
			case 'a':
				++timeout_miliseconds;
				timeout(timeout_miliseconds);
				break;
			case 'z':
				--timeout_miliseconds;
				timeout(timeout_miliseconds);
				break;
		}

		GLOBALTIMER.update_auto();
		ct_1.update_from_globaltimer(GLOBALTIMER);
		erase();
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
		refresh();
	} // loop


	printw("\n goodbye \n");

	endwin();
}
