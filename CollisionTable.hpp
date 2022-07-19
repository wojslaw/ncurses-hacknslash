#pragma once
#include <vector>
#include <assert.h>
#include "Vec2d.hpp"



struct CollisionTable{
	std::vector<std::vector<int>> vector_of_bool;
	bool is_blocked_yx( int const y ,  int const x) const;
	int max_y = 0;
	int max_x = 0;
	void resize(int const _size_y , int const _size_x) {
		
	}
	CollisionTable()
		: max_y(0), max_x(0)
	{}

	CollisionTable(int const _size_y , int const _size_x)
		: max_y(_size_y), max_x(_size_x)
	{
		vector_of_bool.resize(max_y);
		for(auto & row : vector_of_bool)  {
			row.resize(max_x);
			for(auto & cell : row) {
				cell = false;
			}
		}
	}

};


	bool 
CollisionTable::is_blocked_yx( int const y ,  int const x) const
{
	assert(y >= 0);
	assert(x >= 0);
	assert(y <= max_y);
	assert(x <= max_x);

	return vector_of_bool.at(y).at(x);
}
