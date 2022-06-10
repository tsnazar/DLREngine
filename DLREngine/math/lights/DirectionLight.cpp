#include "DirectionLight.h"
#include "Lighting.h"

DirectX::XMVECTOR math::DirectionLight::Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toCameraDir,
	const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material)
{
	using namespace DirectX;
	DirectX::XMVECTOR lightColor = DirectX::XMLoadFloat3(&intensity);
	DirectX::XMVECTOR halfWay = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(toLightDir, toCameraDir));

	DirectX::XMVECTOR NdotL = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, toLightDir), DirectX::XMVectorZero());
	DirectX::XMVECTOR NdotH = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, halfWay), DirectX::XMVectorZero());
	DirectX::XMVECTOR HdotL = DirectX::XMVectorMax(DirectX::XMVector3Dot(toLightDir, halfWay), DirectX::XMVectorZero());

	DirectX::XMVECTOR FL = fresnel(material.f0, NdotL);
	DirectX::XMVECTOR FH = fresnel(material.f0, HdotL);

	DirectX::XMVECTOR D = ggx(DirectX::XMVectorMultiply(material.roughness, material.roughness), NdotH);
	DirectX::XMVECTOR G = smith(DirectX::XMVectorMultiply(material.roughness, material.roughness), NdotV, NdotH);

	DirectX::XMVECTOR spec = brdfCookTorrance(FH, D, G, NdotV, NdotL, DirectX::XMVectorReplicate(solidAngle));
	DirectX::XMVECTOR diff = brdfLambert(material.albedo, material.metalic, FL);

	return (diff * solidAngle + spec) * lightColor * NdotL;
}