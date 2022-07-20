#include "CollisionTable.hpp"


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
	if(v0.y < 0) {return;}
	if(v0.x < 0) {return;}
	if(v0.y > max_y) {return;}
	if(v0.x > max_x) {return;}
	//assert(v0.y >= 0);
	//assert(v0.x >= 0);
	//assert(v0.y <= max_y);
	//assert(v0.x <= max_x);

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
	max_y = _size_y;
	max_x = _size_x;
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

