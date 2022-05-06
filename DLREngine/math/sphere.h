#pragma once
#include "ray.h"
#include "hit_record.h"

#include <DirectXMath.h>

namespace math
{
	struct sphere
	{
		DirectX::XMFLOAT3 center;
		float radius;

		sphere(): center(), radius(0) {}
		sphere(DirectX::XMFLOAT3 center, float radius): center(center), radius(radius) {}
		bool hit(const ray& casted_ray, float t_min, float t_max, hit_record& record) const;
	};
}