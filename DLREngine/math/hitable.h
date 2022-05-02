#pragma once
#include "ray.h"

namespace math
{
	struct hit_record
	{
		float t;
		vec3 point;
		vec3 normal;
	};

	struct hitable
	{
		virtual bool hit(const ray& casted_ray, float t_min, float t_max, hit_record& rec) const = 0;
	};
}