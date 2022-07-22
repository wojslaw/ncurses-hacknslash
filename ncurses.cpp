#include "Ncurses.hpp"

void init_colorpairs(void)
{
	start_color();

	init_pair(
			ENUM_COLORPAIR_RED
			,COLOR_RED
			,COLOR_BLACK);

	init_pair(
			ENUM_COLORPAIR_BLUE
			,COLOR_BLUE
			,COLOR_BLACK);

	init_pair(
			 ENUM_COLORPAIR_GREEN
			,COLOR_GREEN
			,COLOR_BLACK);

	init_pair(
			 ENUM_COLORPAIR_MAGENTA
			,COLOR_MAGENTA
			,COLOR_BLACK);

	init_pair(
			ENUM_COLORPAIR_WHITE_ON_BLUE
			,COLOR_WHITE
			,COLOR_BLUE);
}
