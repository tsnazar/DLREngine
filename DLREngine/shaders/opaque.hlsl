#include "globals.hlsli"
#include "lighting.hlsli"

cbuffer PerMesh : register(b1)
{
    float4x4 meshToModel;
}

cbuffer PerMaterial : register(b2)
{
    int g_flags;
    float g_roughness;
    float g_metallic;
    float paddingPMat1;
}

//flags
static const uint f_hasRoughness = 1 << 0;
static const uint f_hasMetallic = 1 << 1;
static const uint f_hasNormals = 1 << 2;
static const uint f_flipNormals = 1 << 3;

struct VS_INPUT {
    float3 inPosition : POS;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBitangent : BITANGENT;
    float4x4 inModelToWorld : MAT;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float3 worldPos : WPOS;
    float2 texCoord : TCOORD;
    float3 normal : NORM;
    float3x3 TBN : TBN;
};
// vertex shader

VS_OUTPUT vs_main(VS_INPUT input) {

    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.inPosition, 1.0);
    output.position = mul(output.position, meshToModel);
    output.position = mul(output.position, input.inModelToWorld);
    output.worldPos = output.position;
    output.position = mul(output.position, g_viewProj);

    output.texCoord = input.inTexCoord;

    output.normal = mul(input.inNormal, meshToModel);
    output.normal = mul(output.normal , input.inModelToWorld);

    if (g_flags & f_hasNormals)
    {
        float3 tangent = mul(input.inTangent, meshToModel);
        tangent = mul(tangent, input.inModelToWorld);

        float3 bitangent = mul(input.inBitangent, meshToModel);
        bitangent = mul(bitangent, input.inModelToWorld);

        float3x3 TBN = float3x3(tangent, bitangent, output.normal);

        output.TBN = TBN;
    }

    return output;
}

///pixel shader bindings

Texture2D g_colorTexture : TEXTURE: register(t0);
Texture2D g_roughnessTexture : ROUGHNESS: register(t1);
Texture2D g_metallicTexture : METALIC: register(t2);
Texture2D g_normalTexture : NORMAL_MAP: register(t3);
TextureCubeArray g_shadowMap : register(t4);

static const float3 basicF0 = float3(0.04, 0.04, 0.04);

///pixel shader

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float3 albedo = g_colorTexture.Sample(g_sampler, input.texCoord);

    float3 resultColor = float3(0, 0, 0);

    float3 GN = normalize(input.normal); // geometryNormal

    if (g_flags & f_hasNormals)
    {
        input.normal = g_normalTexture.Sample(g_sampler, input.texCoord).xyz * 2.0 - 1.0;
        if (g_flags & f_flipNormals)
            input.normal.y = -input.normal.y;

        input.normal = normalize(mul(input.normal, input.TBN));
    }

    float roughness = g_roughness;
    if (g_flags & f_hasRoughness)
        roughness = g_roughnessTexture.Sample(g_sampler, input.texCoord);

    float metallic = g_metallic;
    if (g_flags & f_hasMetallic)
        metallic = g_metallicTexture.Sample(g_sampler, input.texCoord);

    float3 f0 = lerp(basicF0, albedo, metallic);

    float3 N = normalize(input.normal);
    float3 V = normalize(g_cameraPos - input.worldPos);

    float NdotV = max(dot(N, V), MIN_DOT);

    float3 reflection = reflect(-V, N);

    View view;
    view.reflection = reflection;
    view.NdotV = NdotV;

    Material material;
    material.albedo = albedo;
    material.f0 = f0;
    material.roughness = roughness;
    material.metallic = metallic;

    for (uint i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        float3 L = g_lights[i].position - input.worldPos;
        float3 shadowFragPos = input.worldPos;

        resultColor += calculatePointLighting(N, GN, V, L, view, g_lights[i].radius, g_lights[i].radiance, material, visibilityCalculation(N, normalize(L), shadowFragPos, g_shadowMap, i));
    }
    
    float3 diff = float3(0, 0, 0);
    float3 spec = float3(0, 0, 0);
    addEnvironmentReflection(diff, spec, N, view, material);

    resultColor += diff + spec;

    return float4(resultColor.xyz, 1.0);
}