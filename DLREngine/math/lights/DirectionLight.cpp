#include "DirectionLight.h"
#include "Lighting.h"
using namespace DirectX;

XMVECTOR math::DirectionLight::Illuminate(const XMVECTOR& toLightDir, const XMVECTOR& toCameraDir,
	const XMVECTOR& pixelNormal, const XMVECTOR& NdotV, const math::MaterialVectorized& material)
{
	XMVECTOR lightColor = XMLoadFloat3(&intensity);
	XMVECTOR halfWay = XMVector3Normalize(toLightDir + toCameraDir);

	XMVECTOR NdotL = XMVectorMax(XMVector3Dot(pixelNormal, toLightDir), XMVectorZero());
	XMVECTOR NdotH = XMVectorMax(XMVector3Dot(pixelNormal, halfWay), XMVectorZero());
	XMVECTOR HdotL = XMVectorMax(XMVector3Dot(toLightDir, halfWay), XMVectorZero());

	XMVECTOR FL = fresnel(material.f0, NdotL);
	XMVECTOR FH = fresnel(material.f0, HdotL);

	XMVECTOR D = ggx(XMVectorMultiply(material.roughness, material.roughness), NdotH);
	XMVECTOR G = smith(XMVectorMultiply(material.roughness, material.roughness), NdotV, NdotH);

	XMVECTOR spec = brdfCookTorrance(FH, D, G, NdotV, NdotL, XMVectorReplicate(solidAngle));
	XMVECTOR diff = brdfLambert(material.albedo, material.metalic, FL);

	return (diff * solidAngle + spec) * lightColor * NdotL;
}