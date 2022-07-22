#include "BaseEntity.hpp"
// TODO read base entity data from textfile


const struct BaseEntity TABLE_BASEENTITY[] = {
	{
		.ncurses_symbol = '?',
		.name = "[NONE]" ,
		.level = 0 ,
		.stat_life_max = 0 ,
	} ,
	[ID_BaseEntity_human]  = {
		.ncurses_symbol = '@',
		.name = "human" ,
		.level = 1 ,
		.stat_life_max = 40 ,
		.attack_base = 2 ,
		.attack_dice = 4 ,
		.attack_range= 6,
		.defense_base = 1,
		.seconds_movement = 0.20 ,
	} ,
	[ID_BaseEntity_wraith] = {
		.ncurses_symbol = 'w',
		.name = "wraith" ,
		.level = 2 ,
		.stat_life_max = 12 ,
		.attack_base = 1 ,
		.attack_dice = 3 ,
		.attack_range = 1,
		.seconds_movement = 0.30 ,
		.flag_has_collision = false,
	} ,
	[ID_BaseEntity_gthrower] = {
		.ncurses_symbol = 'g',
		.name = "gthrower" ,
		.level = 1 ,
		.stat_life_max = 8 ,
		.attack_base = 0 ,
		.attack_dice = 2 ,
		.attack_range = 7,
		.seconds_movement = 0.30,
	} ,
	[ID_BaseEntity_dickev] = {
		.ncurses_symbol = 'd',
		.name = "dickev" ,
		.level = 1 ,
		.stat_life_max = 8 ,
		.attack_base = 0 ,
		.attack_dice = 3 ,
		.attack_range = 1,
		.seconds_movement = 0.10,
	} ,
	[ID_BaseEntity_orc] = {
		.ncurses_symbol = 'O',
		.name = "orc" ,
		.description = "armored spear-wielding warrior" ,
		.level = 3 ,
		.stat_life_max = 24 ,
		.attack_base = 4 ,
		.attack_dice = 2 ,
		.attack_range = 2,
		.defense_base = 2,
		.seconds_movement = 0.30,
	} ,
	[ID_BaseEntity_eater] = {
		.ncurses_symbol = 'e',
		.name = "eater" ,
		.description = "minor pest that devours everything it can" ,
		.level = 2 ,
		.stat_life_max = 12 ,
		.attack_base = 1 ,
		.attack_dice = 2 ,
		.attack_range = 1,
		.seconds_movement = 0.33 ,
		.flag_destroys_terrain = true ,
		.flag_is_vampiric = true ,
	} ,
	[ID_BaseEntity_bengalov] = {
		.ncurses_symbol = 'B',
		.name = "bengalov" ,
		.description = "powerful beast of great consitution and height" ,
		.level = 5 ,
		.stat_life_max = 50 ,
		.attack_base = 4 ,
		.attack_dice = 4 ,
		.attack_range = 1,
		.defense_base = 1,
		.seconds_movement = 0.33 ,
		.flag_destroys_terrain = true ,
	} ,
	[ID_BaseEntity_devourer] = {
		.ncurses_symbol = 'E',
		.name = "devourer" ,
		.description = "dangerous monster that consumes everything in it's wake" ,
		.level = 5 ,
		.stat_life_max = 30 ,
		.attack_base = 3 ,
		.attack_dice = 5 ,
		.attack_range = 1,
		.defense_base = 1,
		.seconds_movement = 0.33 ,
		.flag_destroys_terrain = true ,
		.flag_is_vampiric = true ,
	} ,
};

#define SIZEOF_TABLE_BASEENTITY (sizeof(TABLE_BASEENTITY)/sizeof(TABLE_BASEENTITY[0]))






	BaseEntity const &
baseentity_ref_from_id(ID_BaseEntity const id)
{
	assert(id >= 1);
	assert(id < SIZEOF_TABLE_BASEENTITY);
	return TABLE_BASEENTITY[id];
}








void check_table_of_baseentity_is_valid(void)
{
	//bool has_repeats = false;
	for(ID_BaseEntity i = 1; i < SIZEOF_TABLE_BASEENTITY; ++i) {
		for(ID_BaseEntity j = 1; j < SIZEOF_TABLE_BASEENTITY; ++j) {
			if(i == j) {
				continue;
			}
			assert(
					TABLE_BASEENTITY[i].ncurses_symbol
					!=
					TABLE_BASEENTITY[j].ncurses_symbol
					);
		}
	}
}


	ID_BaseEntity
ID_BaseEntity_from_symbol(int const symbol)
{
	for(ID_BaseEntity i = 0; i < SIZEOF_TABLE_BASEENTITY; ++i){
		if(TABLE_BASEENTITY[i].ncurses_symbol == symbol) {
			return i;
		}
	}
	return ID_BaseEntity_invalid;
}
