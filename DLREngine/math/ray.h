#pragma once
#include <DirectXMath.h>

namespace math
{
	struct ray
	{
		DirectX::XMFLOAT3 origin, direction;

		ray() : origin(), direction() {}
		ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction) : origin(origin),direction(direction) {}

		DirectX::XMVECTOR point_at_line(float t) const {
			return DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&origin), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), t)); }
	};
}