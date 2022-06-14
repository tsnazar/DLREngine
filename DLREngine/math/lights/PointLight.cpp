#include "PointLight.h"
#include "Lighting.h"

DirectX::XMVECTOR math::PointLight::Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toLightDist, const DirectX::XMVECTOR& toCameraDir,
	const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material)
{
	using namespace DirectX;
	DirectX::XMVECTOR lightColor = DirectX::XMLoadFloat3(&intensity);
	
	DirectX::XMVECTOR solidAngle = (DirectX::XMVectorReplicate(1.0f) -
		(DirectX::XMVectorSqrt(toLightDist * toLightDist - DirectX::XMVectorReplicate(radius) * DirectX::XMVectorReplicate(radius))) / toLightDist);

	bool ints;
	DirectX::XMVECTOR reflection = DirectX::XMVectorAdd(DirectX::XMVectorNegate(toCameraDir), DirectX::XMVectorScale(DirectX::XMVectorMultiply(pixelNormal, NdotV), 2.0f));
	DirectX::XMVECTOR closestPointDir = math::approximateClosestSphereDir(ints, reflection, DirectX::XMVectorCos(DirectX::XM_2PI * solidAngle), toLightDir * toLightDist, toLightDir, toLightDist, DirectX::XMLoadFloat(&radius));
	DirectX::XMVECTOR NdotD = DirectX::XMVectorMax(DirectX::XMVector3Dot(closestPointDir, pixelNormal), DirectX::XMVectorZero());
	math::clampDirToHorizon(closestPointDir, NdotD, pixelNormal, DirectX::XMVectorZero());

	DirectX::XMVECTOR halfWay = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(closestPointDir, toCameraDir));

	DirectX::XMVECTOR NdotL = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, toLightDir), DirectX::XMVectorZero());
	DirectX::XMVECTOR NdotH = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, halfWay), DirectX::XMVectorZero());
	DirectX::XMVECTOR HdotL = DirectX::XMVectorMax(DirectX::XMVector3Dot(closestPointDir, halfWay), DirectX::XMVectorZero());

	DirectX::XMVECTOR FL = fresnel(material.f0, NdotL);
	DirectX::XMVECTOR FH = fresnel(material.f0, HdotL);

	DirectX::XMVECTOR D = ggx(DirectX::XMVectorMultiply(material.roughness, material.roughness), NdotH);
	DirectX::XMVECTOR G = smith(DirectX::XMVectorMultiply(material.roughness, material.roughness), NdotV, NdotH);

	DirectX::XMVECTOR spec = brdfCookTorrance(FH, D, G, NdotV, NdotD, solidAngle);
	DirectX::XMVECTOR diff = brdfLambert(material.albedo, material.metalic, FL);

	return (diff * solidAngle + spec) * lightColor * NdotL;
}


 //without closestSphereDir
//DirectX::XMVECTOR math::PointLight::Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toLightDist, const DirectX::XMVECTOR& toCameraDir,
//	const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material)
//{
//	using namespace DirectX;
//	DirectX::XMVECTOR lightColor = DirectX::XMLoadFloat3(&intensity);
//	DirectX::XMVECTOR halfWay = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(toLightDir, toCameraDir));
//
//	DirectX::XMVECTOR NdotL = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, toLightDir), DirectX::XMVectorZero());
//	DirectX::XMVECTOR NdotH = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, halfWay), DirectX::XMVectorZero());
//	DirectX::XMVECTOR HdotL = DirectX::XMVectorMax(DirectX::XMVector3Dot(toLightDir, halfWay), DirectX::XMVectorZero());
//
//	DirectX::XMVECTOR FL = fresnel(material.f0, NdotL);
//	DirectX::XMVECTOR FH = fresnel(material.f0, HdotL);
//
//	DirectX::XMVECTOR D = ggx(DirectX::XMVectorMultiply(material.roughness, material.roughness), NdotH);
//	DirectX::XMVECTOR G = smith(DirectX::XMVectorMultiply(material.roughness, material.roughness), NdotV, NdotH);
//
//	DirectX::XMVECTOR solidAngle = (DirectX::XMVectorReplicate(1.0f) -
//		(DirectX::XMVectorSqrt(toLightDist * toLightDist - DirectX::XMVectorReplicate(radius) * DirectX::XMVectorReplicate(radius))) / toLightDist);
//
//	DirectX::XMVECTOR spec = brdfCookTorrance(FH, D, G, NdotV, NdotL, solidAngle);
//	DirectX::XMVECTOR diff = brdfLambert(material.albedo, material.metalic, FL);
//
//	return (diff * solidAngle + spec) * lightColor * NdotL;
//}