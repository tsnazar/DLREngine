#include "globals.hlsli"

float computeGrassAngle(float2 instancePos, float2 windDir)
{
    const float MAX_VARIATION = PI;
    float WIND_WAVE_LENGTH = 50;
    float POWER_WAVE_LENGTH = 233;
    const float WIND_OSCILLATION_FREQ = 0.666;
    const float POWER_OSCILLATION_FREQ = 1.0 / 8.0;

    float instanceRandom = frac(instancePos.x * 12345);
    float windCoord = dot(instancePos, windDir);

    float windPhaseVariation = instanceRandom * MAX_VARIATION;
    float windPhaseOffset = windCoord / WIND_WAVE_LENGTH + windPhaseVariation;
    float windOscillation = (sin(windPhaseOffset - WIND_OSCILLATION_FREQ * 2 * PI * g_time) + 1) / 2;

    float powerPhaseOffset = windCoord / POWER_WAVE_LENGTH;
    float powerOscillation = (sin(powerPhaseOffset - POWER_OSCILLATION_FREQ * 2 * PI * g_time) + 1) / 2;

    float minAngle = lerp(0.0, 0.3, powerOscillation);
    float maxAngle = lerp(0.1, 1., powerOscillation);
    return lerp(minAngle, maxAngle, windOscillation);
}

struct VS_INPUT {
    uint inId : SV_VertexId;
    float3 inPosition : POS;
    float inScale : SCALE;
    float2x2 inRotation : ROT;
};

struct VS_OUTPUT {
    float2 tex : TEX;
    float4 position : WPOS;
    float3 worldOffset : WOFF;
    float scale : SCALE;
};

static const float NUM_GRASS_SECTIONS = 3;

// vertex shader
VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.tex = float2(input.inId % 2, ((input.inId % 8) >> 1) / NUM_GRASS_SECTIONS);
    //output.position = float4((output.tex.x - 0.5), (1 - output.tex.y), ZFAR, 1);
    output.position = float4((output.tex.x - 0.5), 0, ZFAR, 1);
    output.position.xyz *= input.inScale;
    output.position.xz = mul(output.position.xz, input.inRotation);
    output.scale = input.inScale;
    output.worldOffset = input.inPosition;

    return output;
}

cbuffer ShadowTransforms : register(b1)
{
    float4x4 g_shadowTransforms[6];
    uint g_sliceOffset;
    float3 padding;
}

cbuffer WindRotation : register(b4)
{
    float4 g_windRotation;
    float4 g_windInvRotation;
}

struct GS_OUTPUT
{
    float4 position : SV_Position;
    float2 uv : UV;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};

//geometry shader
[maxvertexcount(108)]
void gs_main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    float windAngle = computeGrassAngle(input[0].worldOffset.xz, float2(1, 0)) * PI / 2.0;
    windAngle = max(windAngle, MIN_DOT);

    for (uint face = 0; face < 6; ++face)
    {
        for (uint blade = 0; blade < 4; ++blade)
        {
            float angle = blade * 45.0 * PI / 180.0;

            float bladeSin = 0;
            float bladeCos = 0;
            sincos(angle, bladeSin, bladeCos);
            float2x2 bladeRotation = { bladeCos, bladeSin,
                                    -bladeSin, bladeCos };

            for (uint i = 0; i < 3; ++i)
            {
                GS_OUTPUT output = (GS_OUTPUT)0;

                output.position = input[i].position;
                output.position.xz = mul(output.position.xz, bladeRotation);

                float waveSin = 0;
                float waveCos = 0;
                sincos(windAngle * (1 - input[i].tex.y), waveSin, waveCos);
                float2x2 waveRotation = { waveCos, waveSin,
                                         -waveSin, waveCos };
                float2x2 waveRotationInv = { waveCos, -waveSin,
                                             waveSin, waveCos };

                float R = input[i].scale / windAngle;

                output.position.xz = mul(output.position.xz, float2x2(g_windRotation));

                output.position.x += R;
                output.position.xy = mul(output.position.xy, waveRotation);
                output.position.x -= R;
                output.position.xy = mul(output.position.xy, waveRotationInv);

                output.position.xz = mul(output.position.xz, float2x2(g_windInvRotation));

                output.position.xyz += input[i].worldOffset;
                output.position = mul(output.position, g_shadowTransforms[face]);

                output.slice = g_sliceOffset + face;

                output.uv = input[i].tex;

                outputStream.Append(output);
            }
            outputStream.RestartStrip();
        }
    }
}

Texture2D g_opacity : register(t8);

void ps_main(GS_OUTPUT input)
{
    if (g_opacity.Sample(g_sampler, input.uv).r == 0)
        discard;
}
