#include "globals.hlsli"
#include "fullscreen.hlsli"

Texture2DMS<float,4> forwardTex: register(t0);
Texture2D deferredTex: register(t1);

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float3 deferred = deferredTex.Load(int3(input.position.xy, 0));

    float3 forward = float3(0, 0, 0);

    for (uint i = 0; i < 4; ++i)
    {
        float3 pixelColor = forwardTex.Load(int3(input.position.xy, 0), i);
        forward += pixelColor;
    }

    forward /= 4;

    return float4(forward - deferred, 1);
}