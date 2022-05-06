#pragma once
#include <DirectXMath.h>

namespace math
{
	struct ray
	{
		DirectX::XMFLOAT3 origin, direction;

		ray() {}
		ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction) : origin(origin),direction(direction) {}

		void point_at_line(DirectX::XMFLOAT3& point, float t) const {
			DirectX::XMVECTOR vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&origin), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), t));
			return DirectX::XMStoreFloat3(&point, vec); }
	};
}