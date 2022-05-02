#pragma once
#include "hitable.h"

namespace math
{
	struct sphere : public hitable
	{
		vec3 center;
		float radius;

		sphere(): center(), radius(0) {}
		sphere(vec3 center, float radius): center(center), radius(radius) {}
		virtual bool hit(const ray& casted_ray, float t_min, float t_max, hit_record& record) const;
	};
}