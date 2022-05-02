#pragma once
#include "vec3.h"

namespace math
{
	struct ray
	{
		vec3 origin, direction;

		ray() {}
		ray(const vec3& origin, const vec3& direction) : origin(origin),direction(direction) {}

		vec3 point_at_line(float t) const { return origin + t * direction; }
	};
}