#pragma once
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

typedef size_t ID_BaseEntity;
constexpr ID_BaseEntity ID_BaseEntity_none = 0;
constexpr ID_BaseEntity ID_BaseEntity_invalid = -1;

enum ID_BASEENTITY {
	ID_BaseEntity_human  = 1 ,
	ID_BaseEntity_wraith = 2 ,
	ID_BaseEntity_gthrower = 3 ,
	ID_BaseEntity_silverworm = 4 ,
	ID_BaseEntity_orc = 5 ,
	ID_BaseEntity_eater  = 6 ,
	ID_BaseEntity_bengalov  = 7 ,
	ID_BaseEntity_devourer  = 8 ,
};


struct BaseEntity {
	int ncurses_symbol = 0;
	const char * name = 0;
	const char * description = 0;
	int level = 0;
	int stat_life_max = 0;
	int attack_base = 0;
	int attack_dice = 0;
	int attack_range = 0;
	int defense_base = 0;
	double seconds_movement = 0.0;
	double seconds_regen = 60.0;
	//
	//
	bool flag_lives_on_zero_hp = false;
	bool flag_has_collision = true;
	bool flag_destroys_terrain = false;
	bool flag_is_vampiric = false;
};


BaseEntity const& baseentity_ref_from_id(
		ID_BaseEntity );

void check_table_of_baseentity_is_valid(void);

	ID_BaseEntity
ID_BaseEntity_from_symbol(int const symbol);
