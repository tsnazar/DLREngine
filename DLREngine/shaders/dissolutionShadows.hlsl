#include "globals.hlsli"

cbuffer PerMesh : register(b1)
{
    float4x4 meshToModel;
}

struct VS_INPUT {
    float3 inPosition : POS;
    float2 inTexCoord : TEXCOORD;
    float4x4 inModelToWorld : MAT;
    float inAnimationTime : TIME;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 texCoord : TCOORD;
    float animationTime : TIME;
};
// vertex shader

VS_OUTPUT vs_main(VS_INPUT input) {

    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.inPosition, 1.0);
    output.position = mul(output.position, meshToModel);
    output.position = mul(output.position, input.inModelToWorld);

    output.texCoord = input.inTexCoord;

    output.animationTime = input.inAnimationTime;

    return output;
}

struct GS_OUTPUT
{
    float4 position : SV_Position;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
    float2 texCoord : TCOORD;
    float animationTime : TIME;
};

cbuffer ShadowTransforms : register(b1)
{
    float4x4 g_shadowTransforms[6];
    uint g_sliceOffset;
    float3 padding;
}

//geometry shader
[maxvertexcount(18)]
void gs_main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    [unroll] for (uint face = 0; face < 6; ++face)
    {
        [unroll] for (uint i = 0; i < 3; ++i)
        {
            GS_OUTPUT output = (GS_OUTPUT)0;
            output.slice = g_sliceOffset + face;
            output.position = mul(input[i].position, g_shadowTransforms[face]);

            output.texCoord = input[i].texCoord;
            output.animationTime = input[i].animationTime;

            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

///pixel shader bindings

Texture2D g_noiseTexture : register(t8);

///pixel shader

void ps_main(GS_OUTPUT input)
{
    float dissolveNoise = g_noiseTexture.Sample(g_sampler, input.texCoord);
    float step1 = step(dissolveNoise, input.animationTime);

    if (!step1)
        discard;
}