#include "globals.hlsli"
#include "geometryInclude.hlsli"
#include "gpuParticlesInclude.hlsli"

cbuffer PerMesh : register(b1)
{
    float4x4 meshToModel;
}

cbuffer Constants : register(b4)
{
    uint g_bufferSize;
    float g_maxAnimationTime;
    float CPadding1;
    float CPadding2;
};

cbuffer PerMaterial : register(b2)
{
    int g_flags;
    float g_roughness;
    float g_metallic;
    float padding;
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
    uint inObjectID : OBJECTID;
    float3 inSpherePos : SPHEREPOS;
    float inSpawnTime : TIME;
    float inMaxRadius : RADIUS;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 texCoord : TCOORD;
    float3 normal : NORM;
    float3x3 TBN : TBN;
    float animationTime : TIME;
    float vertexCollisionTime : VTIME;
    uint objectID : OBJECTID;
};
// vertex shader

VS_OUTPUT vs_main(VS_INPUT input) 
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.inPosition, 1.0);
    output.position = mul(output.position, meshToModel);

    float sphereToVertexDist = length(output.position.xyz - input.inSpherePos);
    output.animationTime = (g_time - input.inSpawnTime) / g_maxAnimationTime;
    output.vertexCollisionTime = sphereToVertexDist / input.inMaxRadius;

    output.position = mul(output.position, input.inModelToWorld);
    output.position = mul(output.position, g_viewProj);

    output.texCoord = input.inTexCoord;

    output.normal = mul(input.inNormal, meshToModel);
    output.normal = normalize(mul(output.normal, input.inModelToWorld));

    output.objectID = input.inObjectID;

    if (g_flags & f_hasNormals)
    {
        float3 tangent = mul(input.inTangent, meshToModel);
        tangent = normalize(mul(tangent, input.inModelToWorld));

        float3 bitangent = mul(input.inBitangent, meshToModel);
        bitangent = normalize(mul(bitangent, input.inModelToWorld));

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
Texture2D g_noiseTexture : register(t8);

static const float3 EMISSION = float3(8, 0, 0);
static const float EDGE_DELTA = 0.05;
static const float DISSOLVE_DELTA = 0.2;

///pixel shader
struct PS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float2 roughnessMetalness : SV_Target2;
    float4 emission : SV_Target3;
    uint objectID : SV_Target4;
};

PS_OUTPUT ps_main(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace)
{
    PS_OUTPUT output = (PS_OUTPUT)0;

    float dissolveNoise = g_noiseTexture.Sample(g_sampler, input.texCoord);

    float edge = step(input.vertexCollisionTime, input.animationTime);

    float dissolveAnimationTime = (input.animationTime - input.vertexCollisionTime - EDGE_DELTA) / DISSOLVE_DELTA;

    if (input.animationTime > (input.vertexCollisionTime + EDGE_DELTA) && dissolveAnimationTime >= dissolveNoise)
        discard;

    output.albedo = g_colorTexture.Sample(g_sampler, input.texCoord) * (1 - edge);

    if (!isFrontFace)
    {
        input.normal =  -input.normal;
        input.TBN[2] = input.normal;
    }

    output.normal.zw = packOctahedron(input.normal);

    if (g_flags & f_hasNormals)
    {
        input.normal = g_normalTexture.Sample(g_sampler, input.texCoord).xyz * 2.0 - 1.0;
        if (g_flags & f_flipNormals)
            input.normal.y = -input.normal.y;

        input.normal = normalize(mul(input.normal, input.TBN));
    }

    output.normal.xy = packOctahedron(input.normal);

    float roughness = g_roughness;
    if (g_flags & f_hasRoughness)
        roughness = g_roughnessTexture.Sample(g_sampler, input.texCoord);

    float metallic = g_metallic;
    if (g_flags & f_hasMetallic)
        metallic = g_metallicTexture.Sample(g_sampler, input.texCoord);

    output.roughnessMetalness = float2(roughness, metallic);

    float emissionFading = max((input.animationTime - input.vertexCollisionTime) / EDGE_DELTA, 1) * edge;

    output.emission.rgb = EMISSION * emissionFading * dissolveNoise;
    output.emission.a = output.albedo.a;

    output.objectID = input.objectID;

    return output;
}