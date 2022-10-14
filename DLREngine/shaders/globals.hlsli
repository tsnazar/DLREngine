#ifndef GLOBALS
#define GLOBALS

static const uint MAX_POINT_LIGHTS = 1;
static const float PI = 3.1415926;
static const float MIN_DOT = 0.0005;

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
    float4x4 g_view;
    float4x4 g_invView;
    float4x4 g_proj;
    float4 g_cameraPos;
    float4 g_frustumCorners[3];
    float g_time;
    float3 paddingPFrame;
    PointLight g_lights[MAX_POINT_LIGHTS];
}

cbuffer ShadowMapTransforms : register(b1)
{
    float4x4 g_shadowMapTransforms[MAX_POINT_LIGHTS * 6];
};

cbuffer ShadowMapDimensions : register(b3)
{
    uint g_shadowMapWidth;
    uint g_shadowMapHeight;
    float2 paddingSMap1;
}

SamplerState g_sampler : register(s0);
SamplerState g_linearClampSampler : register(s1);
SamplerComparisonState g_cmpSampler : register(s2);

static const float ZNEAR = 1.0f;
static const float ZFAR = 0.0f;

#endif