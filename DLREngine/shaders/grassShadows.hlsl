#include "globals.hlsli"
#include "grassInclude.hlsli"

struct VS_INPUT {
    uint inId : SV_VertexId;
    float3 inPosition : POS;
    float inScale : SCALE;
    float2x2 inRotation : ROT;
};

struct VS_OUTPUT {
    float4 position : WPOS;
    float2 tex : TEX;
};

cbuffer WindRotation : register(b4)
{
    float4 g_windRotation;
    float4 g_windInvRotation;
}

static const float NUM_GRASS_SECTIONS = 3;
static const float NUM_VERTICES_IN_SQUARE = 6;
static const float NUM_VERTICES_IN_BLADE = NUM_GRASS_SECTIONS * NUM_VERTICES_IN_SQUARE;

// vertex shader
VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    uint bladeIndex = input.inId / NUM_VERTICES_IN_BLADE;

    float angle = bladeIndex * 45.0 * PI / 180.0;

    float bladeSin = 0;
    float bladeCos = 0;
    sincos(angle, bladeSin, bladeCos);
    float2x2 bladeRotation = { bladeCos, bladeSin,
                            -bladeSin, bladeCos };

    uint squareIndex = input.inId % NUM_VERTICES_IN_SQUARE;
    uint squareNumber = input.inId / NUM_VERTICES_IN_SQUARE % NUM_GRASS_SECTIONS;

    output.tex.x = squareIndex % 2;
    output.tex.y = (((squareIndex % 5 % 4) >= 1) + squareNumber) / NUM_GRASS_SECTIONS;

    //output.position = float4((output.tex.x - 0.5), (1 - output.tex.y), ZFAR, 1);
    output.position = float4((output.tex.x - 0.5), 0, ZFAR, 1);
    output.position.xyz *= input.inScale;
    output.position.xz = mul(output.position.xz, bladeRotation);
    output.position.xz = mul(output.position.xz, input.inRotation);

    float windAngle = computeGrassAngle(input.inPosition.xz, normalize(g_windInvRotation.xy)) * PI / 2.0;
    windAngle = max(windAngle, MIN_DOT);

    float waveSin = 0;
    float waveCos = 0;
    sincos(windAngle * (1 - output.tex.y), waveSin, waveCos);
    float2x2 waveRotation = { waveCos, waveSin,
                             -waveSin, waveCos };
    float2x2 waveRotationInv = { waveCos, -waveSin,
                                 waveSin, waveCos };

    float R = input.inScale / windAngle;

    output.position.xz = mul(output.position.xz, float2x2(g_windRotation));

    output.position.x += R;
    output.position.xy = mul(output.position.xy, waveRotation);
    output.position.x -= R;
    output.position.xy = mul(output.position.xy, waveRotationInv);

    output.position.xz = mul(output.position.xz, float2x2(g_windInvRotation));

    output.position.xyz += input.inPosition;

    return output;
}

cbuffer ShadowTransforms : register(b1)
{
    float4x4 g_shadowTransforms[6];
    uint g_sliceOffset;
    float3 padding;
}

struct GS_OUTPUT
{
    float4 position : SV_Position;
    float2 tex : TEX;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};

//geometry shader
[maxvertexcount(108)]
void gs_main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    [unroll]for (uint face = 0; face < 6; ++face)
    {
        [unroll]for (uint i = 0; i < 3; ++i)
        {
            GS_OUTPUT output = (GS_OUTPUT)0;

            output.position = mul(input[i].position, g_shadowTransforms[face]);

            output.slice = g_sliceOffset + face;

            output.tex = input[i].tex;

            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

Texture2D g_opacity : register(t8);

void ps_main(GS_OUTPUT input)
{
    //if (g_opacity.Sample(g_sampler, input.tex).r < ALPHA_THRESHOLD)
    if (grassAlpha(input.tex, g_opacity) < ALPHA_THRESHOLD)
        discard;
}
