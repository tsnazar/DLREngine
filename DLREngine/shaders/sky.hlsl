#include "globals.hlsli"
#include "fullscreen.hlsli"

TextureCube cubeMap : register(t0);

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float4 pixelColor = cubeMap.Sample(g_sampler, input.dir);
    return pixelColor;
}