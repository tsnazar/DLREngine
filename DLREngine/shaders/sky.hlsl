#include "globals.hlsl"

struct VSQuadOut {
    float4 position : SV_Position;
    float2 uv: TexCoord;
};

VSQuadOut vs_main(uint vertexID : SV_VertexID) {
    VSQuadOut result;
    result.uv = float2((vertexID << 1) & 2, vertexID & 2);
    result.position = float4(result.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), ZFAR, 1.0f);
    return result;
}

TextureCube cubeMap : register(t0);

cbuffer Frustum : register(b0)
{
    float4 g_frustumCorners[3];
}

float4 ps_main(VSQuadOut input) : SV_TARGET
{
    float3 pos = g_frustumCorners[0] + input.uv.x * g_frustumCorners[1] + input.uv.y * g_frustumCorners[2];
    float4 pixelColor = cubeMap.Sample(g_sampler, pos);
    return pixelColor;
}