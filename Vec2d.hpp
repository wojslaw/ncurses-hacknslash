#pragma once
#include <algorithm>


typedef struct Vec2d Vec2d;

struct Vec2d {
	int y = 0;
	int x = 0;

	Vec2d(int const _y, int const _x):y(_y),x(_x) {}


	// mutating methods
	void add_vec2d(Vec2d const & v0);
	void subtract_vec2d(Vec2d const & v0);
	void normalize(void);
	void set_zero(void) { y = 0; x = 0; }

	// const methods
	Vec2d as_normalized(void) const;

	int get_length_taxicab(void) const { return std::abs(y) + std::abs(x); }

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
		);


	bool
is_vec2d_position_within_rectangle(
		Vec2d const & v
		,int const y_start
		,int const x_start
		,int const y_max
		,int const x_max
		);


	int
vec2d_highest_distance_between(
		 const Vec2d & v0
		,const Vec2d & v1
		);


Vec2d
vec2d_find_direction_vec2d_from_to(
		Vec2d const & v0
		,Vec2d const & v1
		);
