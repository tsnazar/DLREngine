#pragma once
#include <DirectXMath.h>

namespace math
{
	struct Ray
	{
		DirectX::XMFLOAT3 origin, direction;

		Ray() : origin(), direction() {}
		Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction) : origin(origin), direction(direction) {}

		DirectX::XMVECTOR PointAtLine(float t) const {
			return DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&origin), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), t)); }
	};
}