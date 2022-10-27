#include "globals.hlsli"
#include "lighting.hlsli"
#include "geometryInclude.hlsli"

struct VS_INPUT
{
    uint inVertexID : SV_VertexID;
    uint inInstanceID : SV_InstanceID;
    float3 inPosition : POS;
    float3 inOffset : OFFSET;
    float inRadius : RAD;
    float3 inRadiance : COLOR;
    float inDist : DIST;
};

struct VS_OUTPUT {
    float4 position : SV_Position;
    float2 tex : TEX;
    float3 worldPos : WPOS;
    float radius : RAD;
    float3 radiance : COLOR;
    uint instanceID : SV_InstanceID;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    float2 uv = float2((input.inVertexID << 1) & 2, input.inVertexID & 2);
    output.tex = uv;
    //output.position = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), ZFAR, 1.0f);
    output.position = float4(input.inPosition, 1);
    output.position.xyz *= input.inDist;
    output.position.xyz += input.inOffset;
    output.position = mul(output.position, g_viewProj);
    output.instanceID = input.inInstanceID;

    output.worldPos = input.inOffset;
    output.radius = input.inRadius;
    output.radiance = input.inRadiance;

    return output;
}

cbuffer RenderDimensions : register(b4)
{
    uint g_targetWidth;
    uint g_targetHeight;
    float2 g_RDpadding;
}

Texture2D g_depth : register(t0);
Texture2D g_albedo : register(t1);
Texture2D g_normals : register(t2);
Texture2D g_roughnessMetallic : register(t3);
TextureCubeArray g_shadowMap : register(t4);
Texture2D g_emission : register(t5);

static const float3 basicF0 = float3(0.04, 0.04, 0.04);
static const float power = 4;

float4 ps_main(VS_OUTPUT input) : SV_Target
{
    float depth = g_depth.Load(int3(input.position.xy, 0));
    float3 posWS = worldSpacePostionFromDepth(depth, float2(input.position.x / g_targetWidth, input.position.y / g_targetHeight));
    float4 albedo = g_albedo.Load(int3(input.position.xy, 0));
    float4 normalCompressed = g_normals.Load(int3(input.position.xy, 0));
    float2 roughnessMetallic = g_roughnessMetallic.Load(int3(input.position.xy, 0));
    float4 translucencyAO = g_emission.Load(int3(input.position.xy, 0));

    float3 f0 = lerp(basicF0, albedo.xyz, roughnessMetallic.g);

    float3 N = unpackOctahedron(normalCompressed.xy);
    float3 GN = unpackOctahedron(normalCompressed.zw);

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

    float3 L = input.worldPos - posWS;

    float dist = length(L);
    dist = max(dist, input.radius);
    float lightAngleSin = input.radius / dist;
    float angularCos = sqrt(1.0 - lightAngleSin * lightAngleSin);
    float solidAngle = (1 - angularCos) * 2 * PI;

    L = normalize(L);

    float visibility = visibilityCalculationGrass(GN, L, posWS, g_shadowMap, input.instanceID);

    float NdotL = dot(N, L);

    float3 resultColor = calculatePointLighting(N, GN, V, L, dist, angularCos, lightAngleSin, solidAngle, NdotL, view, input.radius, input.radiance, material, visibility);

    if (NdotL < 0.0f)
        resultColor += input.radiance * solidAngle * translucencyAO.rgb * pow(-NdotL, power) * visibility;

    resultColor *= translucencyAO.a;

    return float4(resultColor, albedo.a);
}