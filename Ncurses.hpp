#pragma once
#include <ncurses.h>
#include <assert.h>

enum ENUM_COLORPAIR {
	ENUM_COLORPAIR_NONE = 0 ,
	ENUM_COLORPAIR_RED  ,
	ENUM_COLORPAIR_BLUE ,
	ENUM_COLORPAIR_GREEN ,
	ENUM_COLORPAIR_MAGENTA ,
	ENUM_COLORPAIR_WHITE_ON_BLUE ,
};

 

#define ATTR_TERRAIN		COLOR_PAIR(ENUM_COLORPAIR_BLUE)
#define ATTR_TERRAIN_GREEN		COLOR_PAIR(ENUM_COLORPAIR_GREEN)
#define ATTR_RECENTLY_HIT		COLOR_PAIR(ENUM_COLORPAIR_RED)
#define ATTR_RECENTLY_HEALED		COLOR_PAIR(ENUM_COLORPAIR_GREEN)
#define ATTR_OUT_OF_MAP_BOUNDS		COLOR_PAIR(ENUM_COLORPAIR_WHITE_ON_BLUE)
#define ATTR_TARGET		WA_REVERSE
#define ATTR_ABILITY_READY		WA_BOLD
#define ATTR_ABILITY_STACK		(WA_BOLD | WA_REVERSE)
#define ATTR_LIFEBAR_NORMAL		(WA_REVERSE)
#define ATTR_HEAVILY_DAMAGED		(WA_ITALIC | COLOR_PAIR(ENUM_COLORPAIR_RED))



void init_colorpairs(void);
