#pragma once
#include <DirectXMath.h>
#include "Ray.h"
#include "Intersection.h"

namespace math
{
	struct Sphere
	{
		DirectX::XMFLOAT3 center;
		float radius;

		Sphere(): center(), radius() {}
		Sphere(DirectX::XMFLOAT3 center, float radius): center(center), radius(radius) {}
		bool Intersect(const Ray& ray, Intersection& record) const;
	};
}