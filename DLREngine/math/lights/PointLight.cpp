#include "PointLight.h"
#include "BRDF.h"

//DirectX::XMVECTOR math::PointLight::Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toLightDist, const DirectX::XMVECTOR& toCameraDir,
//	const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material)
//{
//	DirectX::XMVECTOR lightColor = DirectX::XMLoadFloat3(&intensity);
//	DirectX::XMVECTOR halfWay = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(toLightDir, toCameraDir));
//
//	DirectX::XMVECTOR diff = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, toLightDir), DirectX::XMVectorZero());
//	diff = DirectX::XMVectorMultiply(diff, material.albedo);
//
//	DirectX::XMVECTOR spec = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, halfWay), DirectX::XMVectorZero());
//	spec = DirectX::XMVectorPow(spec, material.glossiness);
//	spec = DirectX::XMVectorMultiply(spec, material.specular);
//
//	DirectX::XMVECTOR k = DirectX::XMVectorDivide(toLightDist, DirectX::XMVectorReplicate(radius));
//	k = DirectX::XMVectorDivide(DirectX::XMVectorReplicate(1.0f), k);
//	k = DirectX::XMVectorMultiply(k, k);
//	lightColor = DirectX::XMVectorMultiply(lightColor, k);
//
//	return DirectX::XMVectorMultiply(DirectX::XMVectorAdd(spec, diff), lightColor);
//}

DirectX::XMVECTOR math::PointLight::Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toLightDist, const DirectX::XMVECTOR& toCameraDir,
	const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material)
{
	using namespace DirectX;
	DirectX::XMVECTOR lightColor = DirectX::XMLoadFloat3(&intensity);
	DirectX::XMVECTOR halfWay = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(toLightDir, toCameraDir));
	
	DirectX::XMVECTOR NdotL = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, toLightDir), DirectX::XMVectorZero());
	DirectX::XMVECTOR NdotH = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, halfWay), DirectX::XMVectorZero());
	//DirectX::XMVECTOR NdotV = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, toCameraDir), DirectX::XMVectorZero());

	//DirectX::XMVECTOR k = DirectX::XMVectorDivide(toLightDist, DirectX::XMVectorReplicate(radius));
	//k = DirectX::XMVectorDivide(DirectX::XMVectorReplicate(1.0f), k);
	//k = DirectX::XMVectorMultiply(k, k);
	//lightColor = DirectX::XMVectorMultiply(lightColor, k);

	DirectX::XMVECTOR FL = fresnel(material.f0, NdotL);
	DirectX::XMVECTOR FH = fresnel(material.f0, NdotH);

	DirectX::XMVECTOR D = ggx(DirectX::XMVectorMultiply(material.roughness, material.roughness), NdotH);
	DirectX::XMVECTOR G = smith(DirectX::XMVectorMultiply(material.roughness, material.roughness), NdotV, NdotH);

	DirectX::XMVECTOR numerator = D * G * FH;
	DirectX::XMVECTOR denominator = DirectX::XMVectorReplicate(4.0f) * NdotV * NdotL + DirectX::XMVectorReplicate(0.0001f);
	DirectX::XMVECTOR spec = numerator / denominator;

	DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(DirectX::XMVectorReplicate(1.0f), FL);
	diff *= (DirectX::XMVectorReplicate(1.0f) - material.metalic);

	return (diff * material.albedo / DirectX::XM_PI + spec) * lightColor * NdotL;
	//return DirectX::XMVectorMultiply(DirectX::XMVectorAdd(spec, diff), lightColor);
}