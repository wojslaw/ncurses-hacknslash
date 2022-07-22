#include "Vec2d.hpp"

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
	if(y > y_max) {
		return false;
	}
	if(x > x_max) {
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


	int
vec2d_distance_taxicab(
		 const Vec2d & v0
		,const Vec2d & v1
		)
{
	return(
			std::abs(v0.y - v1.y)
			+
			std::abs(v0.x - v1.x)
		  );
}





	void
Vec2d::add_vec2d(Vec2d const & v0)
{
	y += v0.y;
	x += v0.x;
}




	void
Vec2d::subtract_vec2d(Vec2d const & v0)
{
	y -= v0.y;
	x -= v0.x;
}




	void
Vec2d::normalize(void)
{
	if(y < -1) { y = -1; }
	if(y > +1) { y = +1; }
	if(x < -1) { x = -1; }
	if(x > +1) { x = +1; }
}




	Vec2d
Vec2d::as_normalized(void) const
{
	Vec2d v = *this;
	v.normalize();
	return v;
}




	Vec2d
vec2d_find_direction_vec2d_from_to(
		 Vec2d const & v0
		,Vec2d const & v1
		)
{
	return Vec2d(
			(v1.y - v0.y)
			,
			(v1.x - v0.x)
			);
}


