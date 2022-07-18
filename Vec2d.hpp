#pragma once


typedef struct Vec2d Vec2d;

struct Vec2d {
	int y = 0;
	int x = 0;

	Vec2d(int const _y, int const _x) {
		y = _y;
		x = _x;
	}

	bool is_within_rectangle(
			int const y_start
			,int const x_start
			,int const y_max
			,int const x_max
			) const;
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



	bool
is_vec2d_position_within_rectangle(
		Vec2d const & v
		,int const y_start
		,int const x_start
		,int const y_max
		,int const x_max
		)
{
	
	if(v.y < y_start) {
		return false;
	}
	if(v.x < x_start) {
		return false;
	}
	if(v.y >= y_max) {
		return false;
	}
	if(v.x >= x_max) {
		return false;
	}
	return true;
}






	bool
Vec2d::is_within_rectangle(
		 int const y_start
		,int const x_start
		,int const y_max
		,int const x_max
		) const
{
	if(y < y_start) {
		return false;
	}
	if(x < x_start) {
		return false;
	}
	if(y >= y_max) {
		return false;
	}
	if(x >= x_max) {
		return false;
	}
	return true;
}



	int
vec2d_highest_distance_between(
		 const Vec2d & v0
		,const Vec2d & v1
		)
{
	return std::max(
			std::abs(v0.y - v1.y)
			,
			std::abs(v0.x - v1.x)
			);
}
