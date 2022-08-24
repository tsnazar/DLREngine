#include "globals.hlsli"
#include "BRDF.hlsli"

cbuffer PerMesh : register(b1)
{
    float4x4 meshToModel;
}

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

    //float4x4 normalModelMat = transpose(inverse(meshToModel));
    //float4x4 normalWorldMat = transpose(inverse(input.inModelToWorld));

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

static const float3 basicF0 = float3(0.04, 0.04, 0.04);
///pixel shader

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float4 albedo = g_colorTexture.Sample(g_sampler, input.texCoord);

    float3 resultColor = float3(0, 0, 0);

    float3 GN = normalize(input.normal); // geometryNormal

    if (g_flags & f_hasNormals)
    {
        input.normal = normalize(g_normalTexture.Sample(g_sampler, input.texCoord).xyz * 2.0 - 1.0);
        if (g_flags & f_flipNormals)
            input.normal.y = -1.0 * input.normal.y;

        input.normal = normalize(mul(input.normal, input.TBN));
    }

    float roughness = g_roughness;
    if (g_flags & f_hasRoughness)
        roughness = g_roughnessTexture.Sample(g_sampler, input.texCoord);

    float metallic = g_metallic;
    if (g_flags & f_hasMetallic)
        metallic = g_metallicTexture.Sample(g_sampler, input.texCoord);

    float3 f0 = lerp(basicF0, albedo, metallic);

    for (uint i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        float dist = length(g_lights[i].position - input.worldPos);
        float radius = g_lights[i].radius;

        float sqrDist = dist * dist;
        float sqrRadius =  radius * radius;

        sqrDist = max(sqrRadius, sqrDist);

        float angularCos = sqrt(1.0 - sqrRadius / sqrDist);

        float lightAngleSin = radius / dist;

        float solidAngle = (1.0 - angularCos) * PI;

        float3 N = normalize(input.normal);
        float3 V = normalize(g_cameraPos - input.worldPos);
        float3 L = normalize(g_lights[i].position - input.worldPos);

        //float NdotL = max(dot(N, L), MIN_DOT);
        float NdotL = dot(N, L);
        if (NdotL < -lightAngleSin)
            continue;

        float GNdotL = dot(GN, L);
        float faddingGeom = 1.0 - saturate((radius - GNdotL * dist) / (2 * radius));
        float faddingNMap = 1.0 - saturate((radius - NdotL * dist) / (2 * radius));

        NdotL = max(NdotL, faddingGeom * lightAngleSin);

        float NdotV = max(dot(N, V), MIN_DOT);

        float3 reflection = 2.0 * N * NdotV - V;

        float3 C = approximateClosestSphereDir(reflection, angularCos, L * dist, L, dist, radius);
        float NdotC = dot(N, C);
        clampDirToHorizon(C, NdotC, N, MIN_DOT);

        float3 radiance = g_lights[i].radiance;

        float3 H = normalize(C + V);

        float NdotH = max(dot(N, H), MIN_DOT);
        float HdotC = max(dot(H, C), MIN_DOT);

        float FL = fresnel(f0, NdotL);
        float FH = fresnel(f0, HdotC);

        float D = ggx(roughness * roughness, NdotH);
        float G = smith(roughness * roughness, NdotV, NdotC);

        float spec = brdfCookTorrance(FH, D, G, NdotV, NdotC, solidAngle);
        float3 diff = brdfLambert(albedo.xyz, metallic, FL);

        resultColor += (diff * solidAngle * NdotL + spec * NdotC) * radiance * faddingGeom * faddingNMap;
        //resultColor += (diff * solidAngle * NdotL + spec * NdotC) * radiance * faddingGeom * faddingNMap;
        //return float4((diff * solidAngle * NdotL + spec * NdotD) * radiance, 1.0);
    }
    
    return float4(resultColor.xyz, 1.0);
}