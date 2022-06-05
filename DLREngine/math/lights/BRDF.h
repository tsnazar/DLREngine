#pragma once
#include <DirectXMath.h>

namespace math
{
	DirectX::XMVECTOR fresnel(const DirectX::XMVECTOR& F0, const DirectX::XMVECTOR& NdotL);
	DirectX::XMVECTOR smith(const DirectX::XMVECTOR& rough2, DirectX::XMVECTOR NdotV, DirectX::XMVECTOR NdotL);
	DirectX::XMVECTOR ggx(const DirectX::XMVECTOR& rough2, const DirectX::XMVECTOR& NdotH, float lightAngleSin = 0, float lightAngleCos = 1);
	DirectX::XMVECTOR mix(const DirectX::XMVECTOR& x, const DirectX::XMVECTOR& y, const DirectX::XMVECTOR& a);
}