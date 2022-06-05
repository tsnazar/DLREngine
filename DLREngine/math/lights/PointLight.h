#pragma once
#include "Material.h"

namespace math
{
	struct PointLight
	{
		DirectX::XMFLOAT3 position, intensity;

		PointLight() : position(), intensity() {}
		PointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 intensity) : position(position), intensity(intensity) {}

		DirectX::XMVECTOR Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toLightDist, const DirectX::XMVECTOR& toCameraDir,
			const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material);
	};
}