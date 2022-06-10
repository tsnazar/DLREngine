#pragma once
#include <DirectXMath.h>
#include <vector>

namespace math
{
	DirectX::XMVECTOR fresnel(const DirectX::XMVECTOR& F0, const DirectX::XMVECTOR& NdotL);
	DirectX::XMVECTOR smith(const DirectX::XMVECTOR& rough2, DirectX::XMVECTOR NdotV, DirectX::XMVECTOR NdotL);
	DirectX::XMVECTOR ggx(const DirectX::XMVECTOR& rough2, const DirectX::XMVECTOR& NdotH, float lightAngleSin = 0, float lightAngleCos = 1);
	DirectX::XMVECTOR mix(const DirectX::XMVECTOR& x, const DirectX::XMVECTOR& y, const DirectX::XMVECTOR& a);
	DirectX::XMVECTOR adjustExposure(const DirectX::XMVECTOR& color, float EV100);
	DirectX::XMVECTOR acesHDRtoLDR(const DirectX::XMVECTOR& hdr);
	DirectX::XMVECTOR brdfCookTorrance(const DirectX::XMVECTOR& F, const DirectX::XMVECTOR& D, const DirectX::XMVECTOR& G, const DirectX::XMVECTOR& NdotV, const DirectX::XMVECTOR& NdotL, const DirectX::XMVECTOR& solidAngle);
	DirectX::XMVECTOR brdfLambert(const DirectX::XMVECTOR& albedo, const DirectX::XMVECTOR& metal, const DirectX::XMVECTOR& F);
	DirectX::XMVECTOR approximateClosestSphereDir(bool& intersects, const DirectX::XMVECTOR& reflectionDir, const DirectX::XMVECTOR& sphereCos,
		const DirectX::XMVECTOR& sphereRelPos, const DirectX::XMVECTOR& sphereDir, const DirectX::XMVECTOR& sphereDist, const DirectX::XMVECTOR& sphereRadius);
	void clampDirToHorizon(DirectX::XMVECTOR& dir, DirectX::XMVECTOR& NdotD, const DirectX::XMVECTOR& normal, const DirectX::XMVECTOR& minNdotD);
	void hemisphereUniformDistribution(std::vector<DirectX::XMVECTOR>& outVec, unsigned int numberOfPoints);
	void branchlessONB(const DirectX::XMVECTOR& n, DirectX::XMVECTOR& outB1, DirectX::XMVECTOR& outB2);
}