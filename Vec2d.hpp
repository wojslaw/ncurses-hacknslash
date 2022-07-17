#pragma once


typedef struct Vec2d Vec2d;

struct Vec2d {
	int y = 0;
	int x = 0;

	Vec2d(int const _y, int const _x) {
		y = _y;
		x = _x;
	}
};



bool
Vec2d_is_equal(
		 const Vec2d & v0
		,const Vec2d & v1
		)
{
	return
		( v0.y == v1.y )
		  &&
		( v0.x == v1.x ) ;
}
