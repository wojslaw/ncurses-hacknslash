#pragma once
#include <ncurses.h>
#include <assert.h>

enum ENUM_COLORPAIR {
	ENUM_COLORPAIR_NONE = 0 ,
	ENUM_COLORPAIR_RECENTLY_HIT  ,
	ENUM_COLORPAIR_TERRAIN ,
};


#define ATTR_TERRAIN COLOR_PAIR(ENUM_COLORPAIR_TERRAIN)
#define ATTR_RECENTLY_HIT COLOR_PAIR(ENUM_COLORPAIR_RECENTLY_HIT)


void init_colorpairs(void)
{
	start_color();

	init_pair(ENUM_COLORPAIR_RECENTLY_HIT,COLOR_RED,COLOR_BLACK);
	init_pair(ENUM_COLORPAIR_TERRAIN,COLOR_BLUE,COLOR_BLACK);
}







void border_around_window(WINDOW * w)
{
	assert(w);
	int y_start,x_start;
	getbegyx(w,y_start,x_start);
	--y_start;
	--x_start;
	int const y_end = y_start + getmaxy(w) +1 ;
	int const x_end = x_start + getmaxx(w) +1 ;
	attron(WA_REVERSE);
	for(int y = y_start; y < y_end; ++y) {
		mvaddch(y, x_start, ' ');
		mvaddch(y, x_end  , ' ');
	}
	for(int x = x_start; x < x_end; ++x) {
		mvaddch(y_start, x , ' ');
		mvaddch(y_end  , x , ' ');
	}
	attroff(WA_REVERSE);
	move(0,0);
	refresh();
}




