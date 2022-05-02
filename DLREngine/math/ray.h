#pragma once
#include "vec3.h"

namespace math
{
	struct ray
	{
		vec3 origin, direction;

		ray() {}
		ray(const vec3& a, const vec3& b) : origin(a),direction(b) {}

		vec3 point_at_line(float t) const { return origin + t * direction; }
	};
}