#include "globals.hlsli"
#include "fullscreen.hlsli"

Texture2DMS<float, 4> g_depth: register(t0);

float ps_main(VS_OUTPUT input) : SV_Depth
{
    float result = 1.0;

    for (uint i = 0; i < 4; ++i)
    {
        float depth = g_depth.Load(int3(input.position.xy, 0), i);
        result = min(result, depth);
    }

    return result;
}