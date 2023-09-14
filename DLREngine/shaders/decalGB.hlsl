#include "globals.hlsli"
#include "geometryInclude.hlsli"

struct VS_INPUT {
    float3 inPosition : POS;
    float3 inColor: COLOR;
    float4x4 inDecalToWorld : MATD;
    float4x4 inWorldToDecal : MATW;
    float3 inRight : RIGHT;
    uint inObjectID : OBJECTID;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    nointerpolation float4x4 worldToDecal : MATW;
    float3 color : COL;
    float3 right : RIGHT;
    nointerpolation uint objectID : OBJECTID;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.inPosition, 1.0);
    output.position.xyz = mul(output.position, input.inDecalToWorld);
    output.position = mul(float4(output.position.xyz, 1.0), g_viewProj);

    output.worldToDecal = input.inWorldToDecal;
    output.right = input.inRight;
    output.objectID = input.inObjectID;

    output.color = input.inColor;

    return output;
}

struct PS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float2 roughnessMetalness : SV_Target2;
    float4 emission : SV_Target3;
};

cbuffer RenderDimensions : register(b2)
{
    uint g_targetWidth;
    uint g_targetHeight;
    float2 g_RDpadding;
}

Texture2D g_depth : register(t0);
Texture2D g_normalsGB : register(t1);
Texture2D g_normalsDecal : register(t2);
Texture2D<uint> g_objectIDGB : register(t3);

PS_OUTPUT ps_main(VS_OUTPUT input) : SV_TARGET
{
    PS_OUTPUT output = (PS_OUTPUT)0;

    uint objectID = g_objectIDGB.Load(int3(input.position.xy, 0));

    if (objectID != input.objectID)
        discard;

    float depth = g_depth.Load(int3(input.position.xy, 0));
    float3 posWS = worldSpacePositionFromDepth(depth, float2(input.position.x / g_targetWidth, input.position.y / g_targetHeight));
    float3 posDecal = mul(float4(posWS, 1.0), input.worldToDecal);

    posDecal.x += 0.5;
    posDecal.y = 0.5 - posDecal.y;
    posDecal.z += 0.5;

    if (posDecal.x < 0 || posDecal.x > 1)
        discard;
    else if (posDecal.y < 0 || posDecal.y > 1)
        discard;
    else if (posDecal.z < 0 || posDecal.z > 1)
        discard;

    float4 decalNormal = g_normalsDecal.Sample(g_sampler, posDecal.xy);

    if (decalNormal.a < 0.90)
        discard;

    float4 compressedNormals = g_normalsGB.Load(int3(input.position.xy, 0));

    decalNormal.xyz = decalNormal.xyz * 2.0 - 1.0;
    decalNormal.y = -decalNormal.y;
    
    decalNormal = normalize(decalNormal);

    float3 normal = unpackOctahedron(compressedNormals.xy);
    float3 geometryNormal = unpackOctahedron(compressedNormals.zw);

    float3 tangent = normalize(input.right - normal * dot(normal, input.right));
    float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    decalNormal.xyz = normalize(mul(decalNormal, TBN));

    output.normal.xy = packOctahedron(normalize(decalNormal.xyz + normal));
    output.normal.zw = compressedNormals.zw;

    output.albedo.rgb = input.color;
    output.albedo.a = 1.0;

    output.roughnessMetalness = float2(0.2, 0.0);

    return output;
}