#include "Lighting.h"
#include <algorithm>
using namespace DirectX;

namespace math
{
	const float GOLDEN_RATIO = (1.f + sqrtf(5.f)) / 2.f;
	

	XMVECTOR fresnel(const XMVECTOR& F0, const XMVECTOR& NdotL)
	{
		return F0 + (XMVectorReplicate(1.0f) - F0) * XMVectorPow(XMVectorReplicate(1.0f) - NdotL, XMVectorReplicate(5.0f));
	}

	XMVECTOR smith(const XMVECTOR& rough2, XMVECTOR NdotV, XMVECTOR NdotL)
	{
		NdotL *= NdotL;
		NdotV *= NdotV;

		return XMVectorReplicate(2.0f) / (XMVectorSqrt(XMVectorReplicate(1.0f) + rough2 * (XMVectorReplicate(1.0f) - NdotV) / NdotV) 
			+ XMVectorSqrt(XMVectorReplicate(1.0f) + rough2 * (XMVectorReplicate(1.0f) - NdotL) / NdotL));
	}

	XMVECTOR ggx(const XMVECTOR& rough2, const XMVECTOR& NdotH, float lightAngleSin, float lightAngleCos)
	{
		XMVECTOR denom = NdotH * NdotH * (rough2 - XMVectorReplicate(1.0f)) + XMVectorReplicate(1.0f);
		denom = XM_PI * denom * denom;
		return rough2 / denom;
	}

	XMVECTOR mix(const XMVECTOR& x, const XMVECTOR& y, const XMVECTOR& a)
	{
		return x * (XMVectorReplicate(1.0f) - a) + y * a;
	}

	XMVECTOR adjustExposure(const XMVECTOR& color, float EV100)
	{
		float LMax = (78.0f / (0.65f * 100.0f)) * powf(2.0f, EV100);
		return color * XMVectorReplicate(1.0f / LMax);
	}

	XMVECTOR acesHDRtoLDR(const XMVECTOR& hdr)
	{
		const XMMATRIX m1 = XMMatrixSet(0.59719f, 0.07600f, 0.02840f, 0.0f, 0.35458f, 0.90834f, 0.13383f, 0.0f, 0.04823f, 0.01566f, 0.83777f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		const XMMATRIX m2 = XMMatrixSet(1.60475f, -0.10208, -0.00327f, 0.0f, -0.53108f, 1.10813, -0.07276f, 0.0f, -0.07367f, -0.00605, 1.07602f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

		XMVECTOR v = XMVector3Transform(hdr, m1);
		XMVECTOR a = v * (v + XMVectorReplicate(0.0245786f)) - XMVectorReplicate(0.000090537f);
		XMVECTOR b = v * (v * XMVectorReplicate(0.983729f) + XMVectorReplicate(0.4329510f)) + XMVectorReplicate(0.238081f);
		XMVECTOR ldr = XMVectorClamp(XMVector3Transform( a / b, m2), XMVectorZero(), XMVectorReplicate(1.0f));
		return ldr;
	}

	XMVECTOR brdfCookTorrance(const XMVECTOR& F, const XMVECTOR& D, const XMVECTOR& G, const XMVECTOR& NdotV, const XMVECTOR& NdotL, const XMVECTOR& solidAngle)
	{
		XMVECTOR k = XMVectorMin((D * solidAngle) / (XMVectorReplicate(4.0f) * NdotV * NdotL + XMVectorReplicate(0.0001f)), XMVectorReplicate(1.0f));
		return F * G * k;
	}

	XMVECTOR brdfLambert(const XMVECTOR& albedo, const XMVECTOR& metal, const XMVECTOR& F)
	{
		return  (XMVectorReplicate(1.0f) - F) * (XMVectorReplicate(1.0f) - metal) * albedo / XM_PI;
	}

	XMVECTOR approximateClosestSphereDir(bool& intersects, const XMVECTOR& reflectionDir, const XMVECTOR& sphereCos,
		const XMVECTOR& sphereRelPos, const XMVECTOR& sphereDir, const XMVECTOR& sphereDist, const XMVECTOR& sphereRadius)
	{
		XMVECTOR RdotS = XMVector3Dot(reflectionDir, sphereDir);

		intersects = XMVectorGetX(XMVectorGreaterOrEqual(RdotS, sphereCos));
		if (intersects) return reflectionDir;
		if(XMVectorGetX(RdotS) < 0.0f) return sphereDir;

		XMVECTOR closestPointDir = XMVector3Normalize(reflectionDir * sphereDist * RdotS - sphereRelPos);
		return XMVector3Normalize(sphereRelPos + sphereRadius * closestPointDir);
	}

	void clampDirToHorizon(XMVECTOR& dir, XMVECTOR& NdotD, const XMVECTOR& normal, const XMVECTOR& minNdotD)
	{
		if (XMVectorGetX(XMVectorLess(NdotD, minNdotD)))
		{
			dir = XMVector3Normalize(dir + (minNdotD - NdotD) * normal);
			NdotD = minNdotD;
		}
	}

	void hemisphereUniformDistribution(std::vector<XMVECTOR>& outVec, uint32_t numberOfPoints)
	{
		outVec.clear();
		outVec.reserve(numberOfPoints);

		for (uint32_t i = 0; i < numberOfPoints; ++i)
		{
			float theta = XM_2PI * i / GOLDEN_RATIO;
			float cosPhi = 1 - (i + 0.5f) / numberOfPoints;
			float sinPhi = sqrtf(1 - cosPhi * cosPhi);
			outVec.push_back(XMVectorSet(cosf(theta) * sinPhi, sinf(theta) * sinPhi, cosPhi, 0.0f));
		}
	}

	void branchlessONB(const XMVECTOR& n, XMVECTOR& outB1, XMVECTOR& outB2)
	{
		float x = XMVectorGetX(n);
		float y = XMVectorGetY(n);
		float z = XMVectorGetZ(n);
		float sign = copysignf(1.0f, z);

		const float a = -1.0f / (sign + z);
		const float b = x * y * a;
		outB1 = XMVectorSet(1.0f + sign * x * x * a, sign * b, -sign * x, 0.0f);
		outB2 = XMVectorSet(b, sign + y * y * a, -y, 0.0f);
	}

	void basisFromDir(XMVECTOR& right, XMVECTOR& top, const XMVECTOR& dir)
	{
		float x = XMVectorGetX(dir);
		float y = XMVectorGetY(dir);
		float z = XMVectorGetZ(dir);

		float k = 1.0 / std::max(1.0f + z, 0.00001f);
		float a = y * k;
		float b = y * a;
		float c = -x * a;
		right = XMVectorSet(z + b, c, -x, 0.0f);
		top = XMVectorSet(c, 1.0 - b, -y, 0.0f);
	}
}