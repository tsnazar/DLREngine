#pragma once
#include <DirectXMath.h>

#include "Ray.h"
#include "Intersection.h"

namespace math
{
	struct Plane
	{
		DirectX::XMFLOAT3 pos, normal;

		Plane() : pos(), normal() {}
		Plane(DirectX::XMFLOAT3 p,  DirectX::XMFLOAT3 n) :pos(p), normal(n) {}

		bool Intersect(const Ray& ray, Intersection& record) const;
	};
}