#pragma once
#include <ncurses.h>

enum ENUM_COLORPAIR {
	ENUM_COLORPAIR_NONE = 0 ,
	ENUM_COLORPAIR_TERRAIN = 2 ,
};


#define ATTR_TERRAIN COLOR_PAIR(ENUM_COLORPAIR_TERRAIN)


void init_colorpairs(void)
{
	start_color();

	//
	init_pair(ENUM_COLORPAIR_TERRAIN,COLOR_BLUE,COLOR_BLACK);
}
