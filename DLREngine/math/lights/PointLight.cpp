#include "PointLight.h"
#include "Lighting.h"
using namespace DirectX;

XMVECTOR math::PointLight::Illuminate(const XMVECTOR& toLightDir, const XMVECTOR& toLightDist, const XMVECTOR& toCameraDir,
	const XMVECTOR& pixelNormal, const XMVECTOR& NdotV, const math::MaterialVectorized& material)
{
	XMVECTOR lightColor = XMLoadFloat3(&intensity);

	XMVECTOR squareRadius = XMVectorReplicate(radius);
	squareRadius *= squareRadius;
	XMVECTOR squareDistance = toLightDist * toLightDist;

	XMVECTOR solidAngle = XMVectorReplicate(1.0f) - XMVectorSqrt(XMVectorReplicate(1.0f) - squareRadius / squareDistance);

	XMVECTOR angularCos = XMVectorReplicate(1.0f) - (XMVectorReplicate(2.0f) * squareRadius) / squareDistance;

	bool ints;
	XMVECTOR reflection = XMVectorAdd(XMVectorNegate(toCameraDir), XMVectorScale(XMVectorMultiply(pixelNormal, NdotV), 2.0f));
	XMVECTOR closestPointDir = math::approximateClosestSphereDir(ints, reflection, angularCos, toLightDir * toLightDist, toLightDir, toLightDist, XMLoadFloat(&radius));
	XMVECTOR NdotD = XMVector3Dot(closestPointDir, pixelNormal);
	math::clampDirToHorizon(closestPointDir, NdotD, pixelNormal, XMVectorZero());

	XMVECTOR halfWay = XMVector3Normalize(XMVectorAdd(closestPointDir, toCameraDir));

	XMVECTOR NdotL = XMVectorMax(XMVector3Dot(pixelNormal, toLightDir), XMVectorZero());
	XMVECTOR NdotH = XMVectorMax(XMVector3Dot(pixelNormal, halfWay), XMVectorZero());
	XMVECTOR HdotL = XMVectorMax(XMVector3Dot(closestPointDir, halfWay), XMVectorZero());

	XMVECTOR FL = fresnel(material.f0, NdotL);
	XMVECTOR FH = fresnel(material.f0, HdotL);

	XMVECTOR D = ggx(XMVectorMultiply(material.roughness, material.roughness), NdotH);
	XMVECTOR G = smith(XMVectorMultiply(material.roughness, material.roughness), NdotV, NdotH);

	XMVECTOR spec = brdfCookTorrance(FH, D, G, NdotV, NdotD, solidAngle);
	XMVECTOR diff = brdfLambert(material.albedo, material.metalic, FL);

	return (diff * solidAngle * NdotL + spec * NdotD) * lightColor;
}

 //without closestSphereDir
//XMVECTOR math::PointLight::Illuminate(const XMVECTOR& toLightDir, const XMVECTOR& toLightDist, const XMVECTOR& toCameraDir,
//	const XMVECTOR& pixelNormal, const XMVECTOR& NdotV, const math::MaterialVectorized& material)
//{
//	XMVECTOR lightColor = XMLoadFloat3(&intensity);
//	XMVECTOR halfWay = XMVector3Normalize(XMVectorAdd(toLightDir, toCameraDir));
//
//	XMVECTOR NdotL = XMVectorMax(XMVector3Dot(pixelNormal, toLightDir), XMVectorZero());
//	XMVECTOR NdotH = XMVectorMax(XMVector3Dot(pixelNormal, halfWay), XMVectorZero());
//	XMVECTOR HdotL = XMVectorMax(XMVector3Dot(toLightDir, halfWay), XMVectorZero());
//
//	XMVECTOR FL = fresnel(material.f0, NdotL);
//	XMVECTOR FH = fresnel(material.f0, HdotL);
//
//	XMVECTOR D = ggx(XMVectorMultiply(material.roughness, material.roughness), NdotH);
//	XMVECTOR G = smith(XMVectorMultiply(material.roughness, material.roughness), NdotV, NdotH);
//
//	XMVECTOR solidAngle = (XMVectorReplicate(1.0f) -
//		(XMVectorSqrt(toLightDist * toLightDist - XMVectorReplicate(radius) * XMVectorReplicate(radius))) / toLightDist);
//
//	XMVECTOR spec = brdfCookTorrance(FH, D, G, NdotV, NdotL, solidAngle);
//	XMVECTOR diff = brdfLambert(material.albedo, material.metalic, FL);
//
//	return (diff * solidAngle + spec) * lightColor * NdotL;
//}