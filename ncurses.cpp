#include "Ncurses.hpp"

void init_colorpairs(void)
{
	start_color();

	init_pair(ENUM_COLORPAIR_RECENTLY_HIT,COLOR_RED,COLOR_BLACK);
	init_pair(ENUM_COLORPAIR_TERRAIN,COLOR_BLUE,COLOR_BLACK);
	init_pair(ENUM_COLORPAIR_TERRAIN_GREEN,COLOR_GREEN,COLOR_BLACK);
}
