#pragma once
#include "ray.h"
#include "hit_record.h"

namespace math
{
	struct sphere
	{
		vec3 center;
		float radius;

		sphere(): center(), radius(0) {}
		sphere(vec3 center, float radius): center(center), radius(radius) {}
		bool hit(const ray& casted_ray, float t_min, float t_max, hit_record& record) const;
	};
}