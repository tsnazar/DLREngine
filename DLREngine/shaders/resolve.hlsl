#include "globals.hlsli"
#include "fullscreen.hlsli"

static const float GAMMA = 1.0f / 2.2f;

float3 acesHdr2Ldr(float3 hdr)
{
    static const float3x3 m1 = float3x3(
        0.59719f, 0.07600f, 0.02840f,
        0.35458f, 0.90834f, 0.13383f,
        0.04823f, 0.01566f, 0.83777f
        );

    static const float3x3 m2 = float3x3(
        1.60475f, -0.10208, -0.00327f,
        -0.53108f, 1.10813, -0.07276f,
        -0.07367f, -0.00605, 1.07602f
        );

    float3 v = mul(hdr, m1);
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    float3 ldr = clamp(mul((a / b), m2), 0.0f, 1.0f);
    return ldr;
}

float3 adjustExposure(float3 color, float EV100)
{
    float LMax = (78.0 / (0.65 * 100.0)) * pow(2.0, EV100);
    return color * (1.0f / LMax);
}

Texture2D g_texture : register(t0);

cbuffer Constants : register(b0)
{
    float g_EV100;
    float3 padding;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float4 output = g_texture.Load(int3(input.position.xy, 0));
    output.xyz = adjustExposure(float3(output.xyz), g_EV100);
    output.xyz = acesHdr2Ldr(output.xyz);
    output.xyz = pow(output.xyz, GAMMA);

    return output;
}