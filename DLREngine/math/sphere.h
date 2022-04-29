#pragma once
#include "hitable.h"

namespace math
{
	struct sphere : public hitable
	{
		vec3 center;
		float radius;

		sphere(): center(), radius(0) {}
		sphere(vec3 cen, float r): center(cen), radius(r) {}
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	};
}