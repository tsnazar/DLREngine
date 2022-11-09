#include "globals.hlsli"
#include "fullscreen.hlsli"
#include "lighting.hlsli"
#include "geometryInclude.hlsli"

Texture2D g_depth : register(t0);
Texture2D g_albedo : register(t1);
Texture2D g_normals : register(t2);
Texture2D g_roughnessMetallic : register(t3);
Texture2D g_emission : register(t4);

static const float3 BASICF0 = float3(0.04, 0.04, 0.04);

float4 ps_main(VS_OUTPUT input) : SV_Target
{
    float depth = g_depth.Load(int3(input.position.xy, 0));
    float3 posWS = worldSpacePositionFromDepth(depth, input.tex);
    float4 albedo = g_albedo.Load(int3(input.position.xy, 0));
    float2 normalCompressed = g_normals.Load(int3(input.position.xy, 0));
    float2 roughnessMetallic = g_roughnessMetallic.Load(int3(input.position.xy, 0));

    float3 f0 = lerp(BASICF0, albedo.xyz, roughnessMetallic.g);

    float3 N = unpackOctahedron(normalCompressed);

    float3 V = normalize(g_cameraPos - posWS);

    float NdotV = max(dot(N, V), MIN_DOT);

    float3 reflection = reflect(-V, N);

    View view;
    view.reflection = reflection;
    view.NdotV = NdotV;

    Material material;
    material.albedo = albedo;
    material.f0 = f0;
    material.roughness = roughnessMetallic.r;
    material.metallic = roughnessMetallic.g;

    float3 diff = float3(0, 0, 0);
    float3 spec = float3(0, 0, 0);
    addEnvironmentReflection(diff, spec, N, view, material);

    float3 emission = g_emission.Load(int3(input.position.xy, 0));

    if (emission.x > 0 || emission.y > 0 || emission.z > 0)
    {
        float3 normedEmission = emission / max(emission.x, max(emission.y, max(emission.z, 1.0)));

        emission = lerp(normedEmission * 0.33, emission.xyz, pow(NdotV, 8));
    }

    return float4(diff + spec + emission, 1);
}