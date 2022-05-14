#include "SpotLight.h"

DirectX::XMVECTOR math::SpotLight::Illuminate(const DirectX::XMVECTOR& toLightDir, const DirectX::XMVECTOR& toLightDist, const DirectX::XMVECTOR& toCameraDir,
	const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material)
{
	DirectX::XMVECTOR lightColor = DirectX::XMLoadFloat3(&intensity);
	DirectX::XMVECTOR halfWay = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(toLightDir, toCameraDir));

	DirectX::XMVECTOR theta = DirectX::XMVector3Dot(toLightDir, DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&direction)));
	DirectX::XMVECTOR epsilon = DirectX::XMVectorReplicate(cos(innerRad) - cos(outerRad));
	DirectX::XMVECTOR intensity = DirectX::XMVectorClamp(DirectX::XMVectorDivide(DirectX::XMVectorSubtract(theta, DirectX::XMVectorReplicate(cos(outerRad))), epsilon), DirectX::XMVectorZero(), DirectX::XMVectorReplicate(1.0f));
	
	DirectX::XMVECTOR diff = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, toLightDir), DirectX::XMVectorZero());
	diff = DirectX::XMVectorMultiply(diff, material.albedo);

	DirectX::XMVECTOR spec = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, halfWay), DirectX::XMVectorZero());
	spec = DirectX::XMVectorPow(spec, material.glossiness);
	spec = DirectX::XMVectorMultiply(spec, material.specular);

	DirectX::XMVECTOR k = DirectX::XMVectorDivide(toLightDist, DirectX::XMVectorReplicate(radius));
	k = DirectX::XMVectorDivide(DirectX::XMVectorReplicate(1.0f), k);
	k = DirectX::XMVectorMultiply(k, k);
	lightColor = DirectX::XMVectorMultiply(lightColor, k);
	lightColor = DirectX::XMVectorMultiply(lightColor, intensity);
	return DirectX::XMVectorMultiply(DirectX::XMVectorAdd(spec, diff), lightColor);
}