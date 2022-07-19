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

void init_colorpairs(void);
void border_around_window(WINDOW * w);
