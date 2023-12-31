#include "globals.hlsli"
#include "lighting.hlsli"
#include "grassInclude.hlsli"

struct VS_INPUT {
    uint inId : SV_VertexId;
    float3 inPosition : POS;
    float inScale : SCALE;
    float2x2 inRotation : ROT;
};

struct VS_OUTPUT {
    float2 tex : TEX;
    float4 position : SV_Position;
    float3 normal : NORM;
    float3 worldPos : WPOS;
};

static const float NUM_GRASS_SECTIONS = 3;
static const float NUM_VERTICES_IN_SQUARE = 6;
static const float NUM_VERTICES_IN_BLADE = NUM_GRASS_SECTIONS * NUM_VERTICES_IN_SQUARE;

cbuffer WindRotation : register(b4)
{
    float4 g_windRotation;
    float4 g_windInvRotation;
}

// vertex shader
VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    uint bladeIndex = input.inId / NUM_VERTICES_IN_BLADE;

    float angle = bladeIndex * 45.0 * PI / 180.0;

    float bladeSin = 0;
    float bladeCos = 0;
    sincos(angle, bladeSin, bladeCos);
    float2x2 bladeRotation = { bladeCos, bladeSin,
                            -bladeSin, bladeCos };

    uint squareIndex = input.inId % NUM_VERTICES_IN_SQUARE;
    uint squareNumber = input.inId / NUM_VERTICES_IN_SQUARE % NUM_GRASS_SECTIONS;

    output.tex.x = squareIndex % 2;
    output.tex.y = (((squareIndex % 5 % 4) >= 1) + squareNumber) / NUM_GRASS_SECTIONS;

    //output.position = float4((output.tex.x - 0.5), (1 - output.tex.y), ZFAR, 1);
    output.position = float4((output.tex.x - 0.5), 0, ZFAR, 1);
    output.position.xyz *= input.inScale;
    output.position.xz = mul(output.position.xz, bladeRotation);
    output.position.xz = mul(output.position.xz, input.inRotation);
    
    output.normal = float3(0, 0, -1);
    output.normal.xz = mul(output.normal.xz, bladeRotation);
    output.normal.xz = mul(output.normal.xz, input.inRotation);
    output.normal = normalize(output.normal);

    float windAngle = computeGrassAngle(input.inPosition.xz, normalize(g_windInvRotation.xy)) * PI / 2.0;
    windAngle = max(windAngle, MIN_DOT);

    float waveSin = 0;
    float waveCos = 0;
    sincos(windAngle * (1 - output.tex.y), waveSin, waveCos);
    float2x2 waveRotation = { waveCos, waveSin, 
                             -waveSin, waveCos };
    float2x2 waveRotationInv = { waveCos, -waveSin, 
                                 waveSin, waveCos };
    
    float R = input.inScale / windAngle;

    output.position.xz = mul(output.position.xz, float2x2(g_windRotation));

    output.position.x += R;
    output.position.xy = mul(output.position.xy, waveRotation);
    output.position.x -= R;
    output.position.xy = mul(output.position.xy, waveRotationInv);

    output.position.xz = mul(output.position.xz, float2x2(g_windInvRotation));

    output.position.xyz += input.inPosition;
    output.worldPos = output.position;
    output.position = mul(output.position, g_viewProj);

    return output;
}

Texture2D g_albedo : register(t0);
Texture2D g_roughnessTexture : register(t1);
Texture2D g_metallicTexture : register(t2);
Texture2D g_normalTexture : register(t3);
TextureCubeArray g_shadowMap : register(t4);
Texture2D g_opacity : register(t8);
Texture2D g_ao : register(t9);
Texture2D g_translucency : register(t10);

static const float3 basicF0 = float3(0.04, 0.04, 0.04);

float4 ps_main(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace) : SV_TARGET
{
    float4 resultColor = float4(0, 0, 0, 0);

    //resultColor.a = g_opacity.Sample(g_sampler, input.tex);
    resultColor.a = grassAlpha(input.tex, g_opacity);
    
    if (resultColor.a < ALPHA_THRESHOLD)
        discard;

    float3 albedo = g_albedo.Sample(g_sampler, input.tex);

    input.normal = isFrontFace ? input.normal : -input.normal;
    
    float3 GN = normalize(input.normal);

    float3 bitangent = float3(0, -1, 0);
    float3 tangent = cross(bitangent, input.normal);

    float3x3 TBN = float3x3(tangent, bitangent, input.normal);

    input.normal = g_normalTexture.Sample(g_sampler, input.tex).xyz * 2.0 - 1.0;
    input.normal.y = -input.normal.y;

    float3 N = normalize(mul(input.normal, TBN));

    float roughness = g_roughnessTexture.Sample(g_sampler, input.tex);

    float metallic = g_metallicTexture.Sample(g_sampler, input.tex);

    float3 f0 = lerp(basicF0, albedo, metallic);

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

        float LdotV = pow(max(dot(normalize(-L), V), 0), 4);

        float dist = length(L);
        dist = max(dist, g_lights[i].radius);
        float lightAngleSin = g_lights[i].radius / dist;
        float angularCos = sqrt(1.0 - lightAngleSin * lightAngleSin);

        float visibility = visibilityCalculationGrass(GN, normalize(L), input.worldPos, g_shadowMap, i);

        resultColor.rgb += g_translucency.Sample(g_sampler, input.tex) * g_lights[i].radiance * (1 - angularCos) * 2 * PI * LdotV * visibility;

        resultColor.rgb += calculatePointLighting(N, GN, V, L, view, g_lights[i].radius, g_lights[i].radiance, material, visibility);
    }

    addEnvironmentDiffuse(resultColor.rgb, N, material);

    resultColor *= g_ao.Sample(g_sampler, input.tex);
    
    return resultColor;
}