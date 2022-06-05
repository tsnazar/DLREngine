#pragma once
#include "Material.h"

namespace math
{
	struct DirectionLight
	{
		DirectX::XMFLOAT3 direction, intensity;

		DirectionLight() : direction(), intensity() {}
		DirectionLight(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 intensity) : direction(direction), intensity(intensity) {}

		DirectX::XMVECTOR Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toCameraDir, 
			const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material);
	};
}