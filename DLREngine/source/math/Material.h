#pragma once
#include <DirectXMath.h>

namespace math
{
	struct MaterialVectorized
	{
		DirectX::XMVECTOR albedo, emission, f0, metalic, roughness;
	};

	struct Material
	{
		DirectX::XMFLOAT3 albedo, emission, f0;
		float metalic, roughness;

		Material() : albedo(), emission(), f0(), metalic(), roughness(){}
		Material(DirectX::XMFLOAT3 albedo, DirectX::XMFLOAT3 emission, DirectX::XMFLOAT3 f0, float metalic, float roughness) : albedo(albedo), emission(emission), f0(f0),
			metalic(metalic), roughness(roughness){}

		MaterialVectorized Vectorize() const { return MaterialVectorized{ DirectX::XMLoadFloat3(&albedo), DirectX::XMLoadFloat3(&emission), DirectX::XMLoadFloat3(&f0),
			DirectX::XMVectorReplicate(metalic), DirectX::XMVectorReplicate(roughness) }; }
	};	
}