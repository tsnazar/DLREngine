#ifndef GLOBALS
#define GLOBALS

static const uint MAX_POINT_LIGHTS = 2;

struct PointLight
{
    float3 position;
    float radius;
    float3 radiance;
    float placeholder;
};

cbuffer PerFrame : register(b0)
{
    float4x4 g_viewProj;
    float3 g_cameraPos;
    float shadowMapFarPlane;
    float4 g_frustumCorners[3];
    PointLight g_lights[MAX_POINT_LIGHTS];
}

SamplerState g_sampler : register(s0);
SamplerState g_linearClampSampler : register(s1);

static const float ZNEAR = 1.0f;
static const float ZFAR = 0.0f;

#endif