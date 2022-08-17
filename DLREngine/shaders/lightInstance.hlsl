#include "globals.hlsl"

struct VS_INPUT {
    float3 inPosition : POS;
    float3 inColor: COLOR;
    float4x4 inModelToWorld : MAT;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float3 color : COL;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.inPosition, 1.0);
    output.position = mul(output.position, input.inModelToWorld);
    output.position = mul(output.position, g_viewProj);

    output.color = input.inColor;

    return output;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}