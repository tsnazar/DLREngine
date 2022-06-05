#include "SpotLight.h"
#include "BRDF.h"

DirectX::XMVECTOR math::SpotLight::Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toLightDist, const DirectX::XMVECTOR& toCameraDir,
	const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material)
{
	using namespace DirectX;
	DirectX::XMVECTOR lightColor = DirectX::XMLoadFloat3(&intensity);
	DirectX::XMVECTOR halfWay = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(toLightDir, toCameraDir));

	DirectX::XMVECTOR theta = DirectX::XMVector3Dot(toLightDir, DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&direction)));
	DirectX::XMVECTOR epsilon = DirectX::XMVectorReplicate(cosInnerRad - cosOuterRad);
	DirectX::XMVECTOR intensity = DirectX::XMVectorClamp(DirectX::XMVectorDivide(DirectX::XMVectorSubtract(theta, DirectX::XMVectorReplicate(cosOuterRad)), epsilon), DirectX::XMVectorZero(), DirectX::XMVectorReplicate(1.0f));

	lightColor = DirectX::XMVectorMultiply(lightColor, intensity);

	DirectX::XMVECTOR NdotL = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, toLightDir), DirectX::XMVectorZero());
	DirectX::XMVECTOR NdotH = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, halfWay), DirectX::XMVectorZero());

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
}