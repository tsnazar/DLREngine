#include "Lighting.h"
using namespace DirectX;

namespace math
{
	const float GOLDEN_RATIO = (1.f + sqrtf(5.f)) / 2.f;

	DirectX::XMVECTOR fresnel(const DirectX::XMVECTOR& F0, const DirectX::XMVECTOR& NdotL)
	{
		return F0 + (XMVectorReplicate(1.0f) - F0) * XMVectorPow(XMVectorReplicate(1.0f) - NdotL, DirectX::XMVectorReplicate(5.0f));
	}

	DirectX::XMVECTOR smith(const DirectX::XMVECTOR& rough2, DirectX::XMVECTOR NdotV, DirectX::XMVECTOR NdotL)
	{
		NdotL *= NdotL;
		NdotV *= NdotV;

		return XMVectorReplicate(2.0f) / (XMVectorSqrt(DirectX::XMVectorReplicate(1.0f) + rough2 * (XMVectorReplicate(1.0f) - NdotV) / NdotV) 
			+ DirectX::XMVectorSqrt(XMVectorReplicate(1.0f) + rough2 * (XMVectorReplicate(1.0f) - NdotL) / NdotL));
	}

	DirectX::XMVECTOR ggx(const DirectX::XMVECTOR& rough2, const DirectX::XMVECTOR& NdotH, float lightAngleSin, float lightAngleCos)
	{
		XMVECTOR denom = NdotH * NdotH * (rough2 - XMVectorReplicate(1.0f)) + XMVectorReplicate(1.0f);
		denom = DirectX::XM_PI * denom * denom;
		return rough2 / denom;
	}

	DirectX::XMVECTOR mix(const DirectX::XMVECTOR& x, const DirectX::XMVECTOR& y, const DirectX::XMVECTOR& a)
	{
		return x * (XMVectorReplicate(1.0f) - a) + y * a;
	}

	DirectX::XMVECTOR adjustExposure(const DirectX::XMVECTOR& color, float EV100)
	{
		float LMax = (78.0f / (0.65f * 100.0f)) * powf(2.0f, EV100);
		return color * XMVectorReplicate(1.0f / LMax);
	}

	DirectX::XMVECTOR acesHDRtoLDR(const DirectX::XMVECTOR& hdr)
	{
		const XMMATRIX m1 = XMMatrixSet(0.59719f, 0.07600f, 0.02840f, 0.0f, 0.35458f, 0.90834f, 0.13383f, 0.0f, 0.04823f, 0.01566f, 0.83777f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		const XMMATRIX m2 = XMMatrixSet(1.60475f, -0.10208, -0.00327f, 0.0f, -0.53108f, 1.10813, -0.07276f, 0.0f, -0.07367f, -0.00605, 1.07602f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

		XMVECTOR v = XMVector3Transform(hdr, m1);
		XMVECTOR a = v * (v + XMVectorReplicate(0.0245786f)) - XMVectorReplicate(0.000090537f);
		XMVECTOR b = v * (v * XMVectorReplicate(0.983729f) + XMVectorReplicate(0.4329510f)) + XMVectorReplicate(0.238081f);
		XMVECTOR ldr = XMVectorClamp(XMVector3Transform( a / b, m2), DirectX::XMVectorZero(), XMVectorReplicate(1.0f));
		return ldr;
	}

	DirectX::XMVECTOR brdfCookTorrance(const DirectX::XMVECTOR& F, const DirectX::XMVECTOR& D, const DirectX::XMVECTOR& G, const DirectX::XMVECTOR& NdotV, const DirectX::XMVECTOR& NdotL, const DirectX::XMVECTOR& solidAngle)
	{
		DirectX::XMVECTOR k = XMVectorMin((D * solidAngle) / (XMVectorReplicate(4.0f) * NdotV * NdotL + XMVectorReplicate(0.0001f)), XMVectorReplicate(1.0f));
		return F * G * k;
	}

	DirectX::XMVECTOR brdfLambert(const DirectX::XMVECTOR& albedo, const DirectX::XMVECTOR& metal, const DirectX::XMVECTOR& F)
	{
		return  (DirectX::XMVectorReplicate(1.0f) - F) * (DirectX::XMVectorReplicate(1.0f) - metal) * albedo / DirectX::XM_PI;
	}

	DirectX::XMVECTOR approximateClosestSphereDir(bool& intersects, const DirectX::XMVECTOR& reflectionDir, const DirectX::XMVECTOR& sphereCos,
		const DirectX::XMVECTOR& sphereRelPos, const DirectX::XMVECTOR& sphereDir, const DirectX::XMVECTOR& sphereDist, const DirectX::XMVECTOR& sphereRadius)
	{
		XMVECTOR RdotS = XMVector3Dot(reflectionDir, sphereDir);

		intersects = XMVectorGetX(XMVectorGreaterOrEqual(RdotS, sphereCos));
		if (intersects) return reflectionDir;
		if(XMVectorGetX(XMVectorLess(RdotS, XMVectorZero()))) return sphereDir;

		XMVECTOR closestPointDir = XMVector3Normalize(reflectionDir * sphereDist * RdotS - sphereRelPos);
		return XMVector3Normalize(sphereRelPos + sphereRadius * closestPointDir);
	}

	void clampDirToHorizon(DirectX::XMVECTOR& dir, DirectX::XMVECTOR& NdotD, const DirectX::XMVECTOR& normal, const DirectX::XMVECTOR& minNdotD)
	{
		if (XMVectorGetX(XMVectorLess(NdotD, minNdotD)))
		{
			dir = XMVector3Normalize(dir + (minNdotD - NdotD) * normal);
			NdotD = minNdotD;
		}
	}

	void hemisphereUniformDistribution(std::vector<DirectX::XMVECTOR>& outVec, uint32_t numberOfPoints)
	{
		outVec.clear();
		outVec.reserve(numberOfPoints);

		for (uint32_t i = 0; i < numberOfPoints; ++i)
		{
			float theta = DirectX::XM_2PI * i / GOLDEN_RATIO;
			//float phi = acosf(1 - (i + 0.5f) / numberOfPoints);
			float cosPhi = 1 - (i + 0.5f) / numberOfPoints;
			float sinPhi = sqrtf(1 - cosPhi * cosPhi);
			outVec.push_back(DirectX::XMVectorSet(cosf(theta) * sinPhi, sinf(theta) * sinPhi, cosPhi, 0.0f));
		}
	}

	void branchlessONB(const DirectX::XMVECTOR& n, DirectX::XMVECTOR& outB1, DirectX::XMVECTOR& outB2)
	{
		float x = XMVectorGetX(n);
		float y = XMVectorGetY(n);
		float z = XMVectorGetZ(n);
		float sign = copysignf(1.0f, z);

		const float a = -1.0f / (sign + z);
		const float b = x * y * a;
		outB1 = XMVectorSet(1.0f + sign * x * x * a, sign * b, -sign * x, 0.0f);
		outB2 = XMVectorSet(-b, -sign - y * y * a, y, 0.0f);
	}
}