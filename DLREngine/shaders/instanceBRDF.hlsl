#include "globals.hlsl"
#include "BRDF.hlsl"

cbuffer PerMesh : register(b1)
{
    float4x4 meshToModel;
}

cbuffer PerMaterial : register(b2)
{
    //bool g_hasRoughness;
    //bool g_hasMetallic;
    int g_flags;
    float g_roughness;
    float g_metallic;
    float placeholder;
}

//flags
static const uint f_hasRoughness = 1 << 0;
static const uint f_hasMetallic = 1 << 1;
static const uint f_hasNormals = 1 << 2;

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
    float3 cameraPos : VIEW;
    float3 lightPos : LIGHT;
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

    output.cameraPos = g_cameraPos;
    output.lightPos = g_lights[0].position;

    if ((g_flags & f_hasNormals) == f_hasNormals)
    {
        float3 tangent = mul(input.inTangent, meshToModel);
        tangent = mul(tangent, input.inModelToWorld);

        tangent = (tangent - output.normal * dot(output.normal, tangent));

        float3 bitangent = mul(input.inBitangent, meshToModel);
        bitangent = mul(bitangent, input.inModelToWorld);

        float3x3 TBN = float3x3(tangent, bitangent, output.normal);
        float3x3 invTBN = transpose(TBN);

        output.TBN = TBN;

        output.worldPos = mul(output.worldPos, invTBN);
        output.cameraPos = mul(output.cameraPos, invTBN);
        output.lightPos = mul(output.lightPos, invTBN);
    }

    return output;
}

///pixel shader bindings

Texture2D objTexture : TEXTURE: register(t0);
Texture2D objRoughness : ROUGHNESS: register(t1);
Texture2D objMetallic : METALIC: register(t2);
Texture2D objNormalMap : NORMAL_MAP: register(t3);

static const float3 basicF0 = float3(0.04, 0.04, 0.04);
///pixel shader

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float4 pixelColor = objTexture.Sample(g_sampler, input.texCoord);

    if ((g_flags & f_hasNormals) == f_hasNormals)
        input.normal = normalize(objNormalMap.Sample(g_sampler, input.texCoord).xyz * 2.0 - 1.0);

    float roughness = g_roughness;
    if ((g_flags & f_hasRoughness) == f_hasRoughness)
        roughness = objRoughness.Sample(g_sampler, input.texCoord);

    float metallic = g_metallic;
    if ((g_flags & f_hasMetallic) == f_hasMetallic)
        metallic = objMetallic.Sample(g_sampler, input.texCoord);

    float3 f0 = lerp(basicF0, pixelColor, metallic);

    float dist = length(input.lightPos - input.worldPos);

    float sqrDist = dist * dist;
    float sqrRadius = g_lights[0].radius * g_lights[0].radius;

    sqrDist = max(sqrDist, sqrRadius);

    float angularCos = sqrt(1.0 - sqrRadius / sqrDist);

    float solidAngle = 1.0 - angularCos;
    
    float3 N = normalize(input.normal); 
    float3 V = normalize(input.cameraPos - input.worldPos);
    float3 L = normalize(input.lightPos - input.worldPos);

    float NdotV = max(dot(N, V), MIN_DOT);

    float3 reflection = 2.0 * N * NdotV - V;

    float3 closestPointDir = approximateClosestSphereDir(reflection, angularCos, L * dist, L, dist, g_lights[0].radius);
    float NdotD = dot(closestPointDir, N);
    clampDirToHorizon(closestPointDir, NdotD, N, MIN_DOT);

    float3 radiance = g_lights[0].radiance;

    float3 H = normalize(closestPointDir + V);

    float NdotL = max(dot(N, L), MIN_DOT);
    float NdotH = max(dot(N, H), MIN_DOT);
    float HdotL = max(dot(H, closestPointDir), MIN_DOT);

    float FL = fresnel(f0, NdotL);
    float FH = fresnel(f0, HdotL);

    float D = ggx(roughness * roughness, NdotH);
    float G = smith(roughness * roughness, NdotV, NdotD);
    
    float spec = brdfCookTorrance(FH, D, G, NdotV, NdotD, solidAngle);
    float3 diff = brdfLambert(pixelColor.xyz, metallic, FL);

    return float4((diff * solidAngle * NdotL + spec * NdotD) * radiance, 1.0);
}