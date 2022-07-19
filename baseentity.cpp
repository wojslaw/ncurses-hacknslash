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
		.stat_life_max = 25 ,
		.attack_base = 2 ,
		.attack_dice = 4 ,
		.attack_range= 6,
		.seconds_movement = 0.25 ,
	} ,
	[ID_BaseEntity_wraith] = {
		.ncurses_symbol = 'w',
		.name = "wraith" ,
		.level = 2 ,
		.stat_life_max = 12 ,
		.attack_base = 1 ,
		.attack_dice = 3 ,
		.attack_range = 1,
		.seconds_movement = 0.4 ,
		.flag_has_collision = false,
	} ,
	[ID_BaseEntity_goblin] = {
		.ncurses_symbol = 'g',
		.name = "goblin" ,
		.level = 1 ,
		.stat_life_max = 8 ,
		.attack_base = 0 ,
		.attack_dice = 2 ,
		.attack_range = 7,
		.seconds_movement = 0.35,
	} ,
	[ID_BaseEntity_giant] = {
		.ncurses_symbol = 'G',
		.name = "giant" ,
		.level = 4 ,
		.stat_life_max = 40 ,
		.attack_base = 4 ,
		.attack_dice = 8 ,
		.attack_range = 2,
		.defense_base = 3,
		.seconds_movement = 0.40 ,
		.flag_destroys_terrain = true ,
	} ,
	[ID_BaseEntity_rockworm] = {
		.ncurses_symbol = 'r',
		.name = "rockworm" ,
		.level = 2 ,
		.stat_life_max = 12 ,
		.attack_base = 1 ,
		.attack_dice = 2 ,
		.attack_range = 2,
		.seconds_movement = 0.40 ,
		.flag_destroys_terrain = true ,
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
	/* // TODO ensure no repeats */
	/* bool has_repeats = false; */
	/* struct BaseEntity TABLE_BASEENTITY[] = { */
	/* for(ID_BaseEntity i = 0; i < SIZEOF_TABLE_BASEENTITY; ++i) { */
	/* 	for(ID_BaseEntity i = 0; i < SIZEOF_TABLE_BASEENTITY; ++i) { */
	/* 		assert(); */
	/* 	} */
	/* } */
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
