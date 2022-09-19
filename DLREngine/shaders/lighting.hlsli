#ifndef LIGHTING
#define LIGHTING
#include "globals.hlsli"

static const float PI = 3.14;
static const float MIN_DOT = 0.0005;

struct Material
{
    float3 albedo;
    float3 f0;
    float roughness;
    float metallic;
};

struct View
{
    float3 reflection;
    float NdotV;
};

#ifdef IBL
TextureCube g_irradiance : register(t5);
TextureCube g_reflection : register(t6);
Texture2D g_reflectance : register(t7);
#endif

uint getCubemapFace(float3 dir)
{
    uint maxComponentId = abs(dir.x) > abs(dir.y) ? 0u : 1u; 
   
    maxComponentId = abs(dir[maxComponentId]) > abs(dir.z) ? maxComponentId : 2u; 
   
    uint faceId = 2 * maxComponentId + (dir[maxComponentId] < 0.f); 
    
    return faceId;
}

float3 fresnel(float3 F0, float NdotL)
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
    float denom = NdotH * NdotH * (rough2 - 1.0) + 1.0f;
    denom = PI * denom * denom;
    return rough2 / denom;
}

float3 brdfCookTorrance(float3 F, float D, float G, float NdotV, float solidAngle)
{
    float k = min((D * solidAngle / (4.0 * NdotV + 0.0001)), 1.0);
    return F * G * k;
}

float3 brdfLambert(float3 albedo, float metal, float3 F)
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

float shadowCalculation(float3 N, float3 L, float3 fragPos, float3 lightPos, TextureCubeArray tex, float texWidth, uint index)
{
    fragPos += L * 0.005;
    float3 sampleVec = fragPos - lightPos;
    float4x4 mat = g_shadowMapTransforms[index * 6 + getCubemapFace(sampleVec)];
    float4 pos = mul(float4(fragPos, 1.0), mat);
    pos.xyz /= pos.w;
    float currentDepth = pos.z;

    fragPos += N * pos.w * 2 / texWidth;

    sampleVec = fragPos - lightPos;

    float shadow = tex.SampleCmp(g_cmpSampler, float4(sampleVec, index), currentDepth);
    return shadow;
}

#ifdef IBL
void addEnvironmentReflection(inout float3 diffuseReflection, inout float3 specularReflection, float3 N, in View v, in Material m)
{
    diffuseReflection += m.albedo * (1.0 - m.metallic) * g_irradiance.SampleLevel(g_linearClampSampler, N, 0.0);
    float2 reflectanceLUT = g_reflectance.Sample(g_linearClampSampler, float2(m.roughness, v.NdotV));
    float3 reflectance = reflectanceLUT.x * m.f0 + reflectanceLUT.y;
    float3 samplel = g_reflection.SampleLevel(g_linearClampSampler, v.reflection, m.roughness * 9);
    specularReflection += reflectance * samplel;
}
#endif

float3 calculatePointLighting(float3 N, float3 GN, float3 V, float3 L, View view, float radius, float3 radiance, Material material, float shadow)
{
    float dist = length(L);

    dist = max(dist, radius);

    float lightAngleSin = radius / dist;

    float angularCos = sqrt(1.0 - lightAngleSin * lightAngleSin);

    float solidAngle = (1.0 - angularCos) * 2 * PI;

    L = normalize(L);

    float NdotL = dot(N, L);
    if (NdotL < -lightAngleSin)
        return float3(0, 0, 0);

    float GNdotL = dot(GN, L);
    float faddingGeom = 1.0 - saturate((radius - GNdotL * dist) / (2 * radius));
    float faddingNMap = 1.0 - saturate((radius - NdotL * dist) / (2 * radius));

    NdotL = max(NdotL, faddingNMap * lightAngleSin);

    float3 C = approximateClosestSphereDir(view.reflection, angularCos, L * dist, L, dist, radius);
    float NdotC = dot(N, C);
    clampDirToHorizon(C, NdotC, N, MIN_DOT);

    float3 H = normalize(C + V);

    float NdotH = max(dot(N, H), MIN_DOT);
    float HdotC = max(dot(H, C), MIN_DOT);

    float3 FL = fresnel(material.f0, NdotL);
    float3 FH = fresnel(material.f0, HdotC);

    float D = ggx(material.roughness * material.roughness, NdotH);
    float G = smith(material.roughness * material.roughness, view.NdotV, NdotC);

    float3 spec = brdfCookTorrance(FH, D, G, view.NdotV, solidAngle);
    float3 diff = brdfLambert(material.albedo.xyz, material.metallic, FL);

    return float3((diff * solidAngle * NdotL + spec) * radiance * faddingGeom * faddingNMap * shadow);
}
#endif