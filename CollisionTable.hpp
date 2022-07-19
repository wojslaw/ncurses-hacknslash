#pragma once
#include <vector>
#include <assert.h>
#include "Vec2d.hpp"



struct CollisionTable{
	std::vector<std::vector<int>> vector_of_bool;
	int max_y = 0;
	int max_x = 0;
	int size_y = 0;
	int size_x = 0;

	void resize_and_clear(int const _size_y , int const _size_x);
	void clear(void);
	//
	void set_blocked_vec2d( Vec2d const&  v0 );
	void set_blocked_yx( int const y ,  int const x);

	// const methods
	bool is_blocked_yx( int const y ,  int const x) const;
	bool is_blocked_vec2d( Vec2d const& v0 ) const;

	CollisionTable(int const _size_y , int const _size_x)
		: max_y(_size_y-1), max_x(_size_x-1)
		, size_y(_size_y), size_x(_size_x)
	{
		resize_and_clear(_size_y,_size_x);
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




	bool 
CollisionTable::is_blocked_vec2d( Vec2d const& v0 ) const
{
	assert(v0.y >= 0);
	assert(v0.x >= 0);
	assert(v0.y <= max_y);
	assert(v0.x <= max_x);

	return vector_of_bool.at(v0.y).at(v0.x);
}




	void 
CollisionTable::set_blocked_vec2d( Vec2d const&  v0 )
{
	assert(v0.y >= 0);
	assert(v0.x >= 0);
	assert(v0.y <= max_y);
	assert(v0.x <= max_x);

	vector_of_bool.at(v0.y).at(v0.x) = true;
}




	void 
CollisionTable::set_blocked_yx( int const y ,  int const x)
{
	assert(y >= 0);
	assert(x >= 0);
	assert(y <= max_y);
	assert(x <= max_x);

	vector_of_bool.at(y).at(x) = true;
}




	void
CollisionTable::resize_and_clear(int const _size_y , int const _size_x)
{
	int max_y = _size_y;
	int max_x = _size_x;
	vector_of_bool.resize(max_y);
	for(auto & row : vector_of_bool)  {
		row.resize(max_x);
		for(auto & cell : row) {
			cell = false;
		}
	}
}




	void
CollisionTable::clear(void)
{
	for(auto & row : vector_of_bool)  {
		for(auto & cell : row) {
			cell = false;
		}
	}
}

