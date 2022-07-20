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



