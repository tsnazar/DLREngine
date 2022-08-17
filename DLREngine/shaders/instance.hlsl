#include "globals.hlsl"

cbuffer PerMesh : register(b1)
{
    float4x4 meshToModel;
}

struct VS_INPUT {
    float3 inPosition : POS;
    float2 inTexCoord : TEXCOORD;
    float4x4 mat : MAT;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 texCoord : TCOORD;
};

Texture2D objTexture : TEXTURE: register(t0);

VS_OUTPUT vs_main(VS_INPUT input) {

    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.inPosition, 1.0);
    output.position = mul(output.position, meshToModel);
    output.position = mul(output.position, input.mat);
    output.position = mul(output.position, g_viewProj);

    output.texCoord = input.inTexCoord;

    return output;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET{
    float3 pixelColor = objTexture.Sample(g_sampler, input.texCoord);
    return float4(pixelColor, 1.0f);
}