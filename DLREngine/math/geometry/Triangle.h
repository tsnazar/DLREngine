#pragma once
#include <DirectXMath.h>
#include "Ray.h"
#include "Intersection.h"

namespace math
{
	struct Triangle
	{
		DirectX::XMFLOAT3 vertices[3];

		Triangle() : vertices() {}
		Triangle(DirectX::XMFLOAT3 A, DirectX::XMFLOAT3 B, DirectX::XMFLOAT3 C) : vertices{A, B, C} { }
		bool Intersect(const Ray& ray, Intersection& record) const;
	};
}