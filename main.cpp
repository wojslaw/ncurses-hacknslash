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

	srand(time(NULL));
	raw();
	noecho();
	keypad(stdscr, TRUE);
	refresh();









	int ch = ERR;
	timeout(10);

	for(ch = ERR ;  ch != 'q' ; ch = getch() ) {
		refresh();
	}


	printw("\n goodbye \n");

	endwin();
}
