#include "BRDF.h"

namespace math
{
	DirectX::XMVECTOR fresnel(const DirectX::XMVECTOR& F0, const DirectX::XMVECTOR& NdotL)
	{
		using namespace DirectX;
		return F0 + (XMVectorReplicate(1.0f) - F0) * XMVectorPow(XMVectorReplicate(1.0f) - NdotL, DirectX::XMVectorReplicate(5.0f));
	}

	DirectX::XMVECTOR smith(const DirectX::XMVECTOR& rough2, DirectX::XMVECTOR NdotV, DirectX::XMVECTOR NdotL)
	{
		using namespace DirectX;
		NdotL *= NdotL;
		NdotV *= NdotV;

		return XMVectorReplicate(2.0f) / (XMVectorSqrt(DirectX::XMVectorReplicate(1.0f) + rough2 * (XMVectorReplicate(1.0f) - NdotV) / NdotV) 
			+ DirectX::XMVectorSqrt(XMVectorReplicate(1.0f) + rough2 * (XMVectorReplicate(1.0f) - NdotL) / NdotL));
	}

	DirectX::XMVECTOR ggx(const DirectX::XMVECTOR& rough2, const DirectX::XMVECTOR& NdotH, float lightAngleSin, float lightAngleCos)
	{
		using namespace DirectX;
		DirectX::XMVECTOR denom = NdotH * NdotH * (rough2 - XMVectorReplicate(1.0f)) + XMVectorReplicate(1.0f);
		denom = DirectX::XM_PI * denom * denom;
		return rough2 / denom;
	}

	DirectX::XMVECTOR mix(const DirectX::XMVECTOR& x, const DirectX::XMVECTOR& y, const DirectX::XMVECTOR& a)
	{
		using namespace DirectX;
		return x * (XMVectorReplicate(1.0f) - a) + y * a;
	}
}