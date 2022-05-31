#pragma once
#include <DirectXMath.h>

namespace math
{
	struct MaterialVectorized
	{
		DirectX::XMVECTOR albedo, emission, glossiness, specular;
	};

	struct Material
	{
		DirectX::XMFLOAT3 albedo, emission;
		float glossiness, specular;

		Material() : albedo(), emission(), glossiness(), specular(){}
		Material(DirectX::XMFLOAT3 albedo, DirectX::XMFLOAT3 emission, float glossiness, float specular) : albedo(albedo), emission(emission),
			glossiness(glossiness), specular(specular){}

		MaterialVectorized Vectorize() const { return MaterialVectorized{ DirectX::XMLoadFloat3(&albedo), DirectX::XMLoadFloat3(&emission), DirectX::XMVectorReplicate(glossiness), DirectX::XMVectorReplicate(specular) }; }
	};	
}