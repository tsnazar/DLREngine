#pragma once

static const float PI = 3.14;
static const float MIN_DOT = 0.0005;

struct Material
{
    float3 albedo;
    float3 f0;
    float roughness;
    float metallic;
};

float fresnel(float3 F0, float NdotL)
{
	return F0 + (1.0 - F0) * pow(1.0 - NdotL, 5.0);
}

float smith(float rough2, float NdotV, float NdotL)
{
	NdotL *= NdotL;
	NdotV *= NdotV;

	return 2.0 / (sqrt(1.0 + rough2 * (1 - NdotV) / NdotV) + sqrt(1.0 + rough2 * (1.0 - NdotL) / NdotL));
}

float ggx(float rough2, float NdotH)
{
	float3 denom = NdotH * NdotH * (rough2 - 1.0) + 1.0f;
	denom = PI * denom * denom;
	return rough2 / denom;
}

float brdfCookTorrance(float F, float D, float G, float NdotV, float NdotL, float solidAngle)
{
	float3 k = min((D * solidAngle) / (4.0 * NdotV * NdotL + 0.0001), 1.0);
	return F * G * k;
}

float3 brdfLambert(float3 albedo, float metal, float F)
{
	return  (1.0 - F) * (1.0 - metal) * albedo / PI;
}

float3 approximateClosestSphereDir(float3 reflectionDir, float sphereCos,
	float3 sphereRelPos, float3 sphereDir, float3 sphereDist, float sphereRadius)
{
	float RdotS = dot(reflectionDir, sphereDir);

	if (RdotS >= sphereCos) return reflectionDir;
	if (RdotS < 0.0) return sphereDir;

	float3 closestPointDir = normalize(reflectionDir * sphereDist * RdotS - sphereRelPos);
	return normalize(sphereRelPos + sphereRadius * closestPointDir);
}

void clampDirToHorizon(inout float3 dir, inout float NdotD, float3 normal, float minNdotD)
{
	if (NdotD < minNdotD)
	{
		dir = normalize(dir + (minNdotD - NdotD) * normal);
		NdotD = minNdotD;
	}
}

float3 calculatePointLighting(float3 N, float3 GN, float3 V,  float NdotV, float3 reflection, float3 fragmentPosition, float3 position, float radius, float3 radiance, Material material)
{
    float dist = length(position - fragmentPosition);

    dist = max(dist, radius);

    float lightAngleSin = radius / dist;

    float angularCos = sqrt(1.0 - lightAngleSin * lightAngleSin);

    float solidAngle = (1.0 - angularCos) * 2 * PI;

    float3 L = normalize(position - fragmentPosition);

    float NdotL = dot(N, L);
    if (NdotL < -lightAngleSin)
        return float3(0,0,0);

    float GNdotL = dot(GN, L);
    float faddingGeom = 1.0 - saturate((radius - GNdotL * dist) / (2 * radius));
    float faddingNMap = 1.0 - saturate((radius - NdotL * dist) / (2 * radius));

    NdotL = max(NdotL, faddingNMap * lightAngleSin);

    float3 C = approximateClosestSphereDir(reflection, angularCos, L * dist, L, dist, radius);
    float NdotC = dot(N, C);
    clampDirToHorizon(C, NdotC, N, MIN_DOT);

    float3 H = normalize(C + V);

    float NdotH = max(dot(N, H), MIN_DOT);
    float HdotC = max(dot(H, C), MIN_DOT);

    float FL = fresnel(material.f0, NdotL);
    float FH = fresnel(material.f0, HdotC);

    float D = ggx(material.roughness * material.roughness, NdotH);
    float G = smith(material.roughness * material.roughness, NdotV, NdotC);

    float spec = brdfCookTorrance(FH, D, G, NdotV, NdotC, solidAngle);
    float3 diff = brdfLambert(material.albedo.xyz, material.metallic, FL);

   return float3((diff * solidAngle * NdotL + spec * NdotC) * radiance * faddingGeom * faddingNMap);
}