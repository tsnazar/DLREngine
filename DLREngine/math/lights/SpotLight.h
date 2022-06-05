#pragma once
#include "Material.h"

namespace math
{
	struct SpotLight
	{
		DirectX::XMFLOAT3 position, direction, intensity;
		float radius, innerRad, outerRad;
		float cosInnerRad, cosOuterRad;

		SpotLight() : position(), direction(), intensity(), radius(), innerRad(), outerRad() {}
		SpotLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 intensity, float radius, float innerRad, float outerRad)
			: position(position), direction(direction), intensity(intensity), radius(radius), innerRad(innerRad), outerRad(outerRad), cosInnerRad(cosf(innerRad)), cosOuterRad(cosf(outerRad)) {}

		DirectX::XMVECTOR Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toLightDist, const DirectX::XMVECTOR& toCameraDir,
			const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material);
	};
}