#pragma once
#include <DirectXMath.h>

#include "ray.h"
#include "hit_record.h"

namespace math
{
	struct plane
	{
		DirectX::XMFLOAT3 point, normal;

		plane() : point(), normal() {}
		plane(DirectX::XMFLOAT3 p,  DirectX::XMFLOAT3 n) :point(p), normal(n) {}

		bool hit(const ray& casted_ray, float t_min, float t_max, hit_record& record) const;
	};
}