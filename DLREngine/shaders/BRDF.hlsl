static const float PI = 3.14;
static const float MIN_DOT = 0.0005;

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