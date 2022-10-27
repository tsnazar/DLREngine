#ifndef GEOMETRY_FUNCTIONS
#define GEOMETRY_FUNCTIONS

float2 nonZeroSign(float2 v)
{
	return float2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0);
}

float2 packOctahedron(float3 v)
{
	float2 p = v.xy / (abs(v.x) + abs(v.y) + abs(v.z));
	return v.z <= 0.0 ? (float2(1.0, 1.0) - abs(p.yx)) * nonZeroSign(p) : p;
}

float3 unpackOctahedron(float2 oct)
{
	float3 v = float3(oct, 1.0 - abs(oct.x) - abs(oct.y));
	if (v.z < 0) v.xy = (float2(1.0, 1.0) - abs(v.yx)) * nonZeroSign(v.xy);
	return normalize(v);
}

float3 worldSpacePostionFromDepth(float depth, float2 tex)
{
	float4 posCS = float4(tex.x * 2.0 - 1.0, (1.0 - tex.y) * 2.0 - 1.0, depth, 1.0);
	float4 posVS = mul(posCS, g_invProj);

	posVS /= posVS.w;

	float4 posWS = mul(posVS, g_invView);

	return posWS.xyz;
}

#endif