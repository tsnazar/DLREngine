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
    float3 worldPos : WPOS;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};

cbuffer ShadowTransforms : register(b1)
{
    float4x4 g_shadowTransforms[6];
}

//geometry shader
[maxvertexcount(18)]
void gs_main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    for (uint face = 0; face < 6; ++face)
    {
        for (uint i = 0; i < 3; ++i)
        {
            GS_OUTPUT output = (GS_OUTPUT)0;
            output.slice = face;
            output.worldPos = input[i].position;
            output.position = mul(input[i].position, g_shadowTransforms[face]);
            //output.position.z = length(input[i].position - lightPos) * output.position.w / farPlane;

            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

//struct PS_OUTPUT {
//    float depth : SV_Depth;
//};
//
//PS_OUTPUT ps_main(GS_OUTPUT input)
//{
//    PS_OUTPUT output = (PS_OUTPUT)0;
//    output.depth = length(input.worldPos - lightPos);
//    output.depth /= farPlane;
//
//    return output;
// }


void ps_main()
{
}