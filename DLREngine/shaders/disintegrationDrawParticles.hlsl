#include "globals.hlsli"
#include "gpuParticlesInclude.hlsli"

cbuffer Constants : register(b1)
{
    uint g_bufferSize;
    float g_maxAnimationTime;
    float CPadding1;
    float CPadding2;
};

struct VS_INPUT {
    uint inVertexID : SV_VertexId;
    uint inInstanceID : SV_InstanceId;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 tex : TEX;
    float3 worldPos : WPOS;
    float3 viewPos : NORMAL;
    nointerpolation float animationTime : TIME;
};

StructuredBuffer<Particle> g_gpuParticles : register(t0);
StructuredBuffer<int> g_range : register(t1);

static const float SIZE = 0.03;

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    uint index = (g_range[1] + input.inInstanceID) % g_bufferSize;

    float3 pos = g_gpuParticles[index].pos;

    output.tex = float2(input.inVertexID % 2, (input.inVertexID % 4) >> 1);
    output.position = float4((output.tex.x - 0.5) * 2, -(output.tex.y - 0.5) * 2, 0, 1);
    output.position.xy *= SIZE;
    output.position.xyz += mul(float4(pos, 1), g_view);
    output.viewPos.xyz = output.position.xyz;
    output.worldPos.xyz = mul(output.position, g_invView);
    output.position = mul(output.position, g_proj);
    output.animationTime = (g_time - g_gpuParticles[index].spawnTime) / g_maxAnimationTime;

    return output;
}

Texture2D g_normalTexture : register(t0);

static const float4 EMISSION = float4(4, 1, 0, 1);
static const float FADING_DELTA = 0.1;

float4 ps_main(VS_OUTPUT input) : SV_Target
{
    float3 cameraDir = normalize(-input.viewPos);

    float4 normal = g_normalTexture.Sample(g_sampler, input.tex);

    if (normal.a < 0.90)
        discard;

    float alphaVisibility = 1;
    if (input.animationTime < FADING_DELTA)
        alphaVisibility = input.animationTime / FADING_DELTA;
    else if(input.animationTime > 1 - FADING_DELTA)
        alphaVisibility = (1 - input.animationTime) / (1 - FADING_DELTA);

    normal.xyz = normalize(normal.xyz * 2.0 - 1.0);
    normal.y = -normal.y;
    normal.z = -normal.z;

    float NdotV = dot(cameraDir, normal);
    return float4(EMISSION.xyz, pow(max(0.0, NdotV), 8) * alphaVisibility);
}