#include "globals.hlsli"

cbuffer PerMesh : register(b1)
{
    float4x4 g_meshToModel;
}

struct VS_INPUT {
    float3 inPosition : POS;
    float4x4 inModelToWorld : MAT;
};

struct VS_OUTPUT {
    float4 position : WPOS;
};

// vertex shader
VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.inPosition, 1.0);
    output.position = mul(output.position, g_meshToModel);
    output.position = mul(output.position, input.inModelToWorld);

    return output;
}

struct GS_OUTPUT
{
    float4 position : SV_Position;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
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
    [unroll]for (uint face = 0; face < 6; ++face)
    {
        [unroll]for (uint i = 0; i < 3; ++i)
        {
            GS_OUTPUT output = (GS_OUTPUT)0;
            output.slice = g_sliceOffset + face;
            output.position = mul(input[i].position, g_shadowTransforms[face]);

            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

void ps_main()
{
}