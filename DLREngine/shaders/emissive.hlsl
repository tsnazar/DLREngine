#include "globals.hlsli"

struct VS_INPUT {
    float3 inPosition : POS;
    float3 inNormal : NORM;
    float3 inColor: COLOR;
    float4x4 inModelToWorld : MAT;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float3 worldPos : WORLD;
    float3 color : COL;
    float3 normal : NORMAL;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.inPosition, 1.0);
    output.worldPos = mul(output.position, input.inModelToWorld);
    output.position = mul(float4(output.worldPos, 1.0), g_viewProj);

    output.normal = mul(input.inNormal, input.inModelToWorld);

    output.color = input.inColor;

    return output;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 cameraDir = normalize(g_cameraPos - input.worldPos);

    float3 normedEmission = input.color / max(input.color.x, max(input.color.y, max(input.color.z, 1.0)));
    float NdotV = dot(cameraDir, normal);

    return float4(lerp(normedEmission * 0.33, input.color.xyz, pow(max(0.0, NdotV), 8)), 1);
}