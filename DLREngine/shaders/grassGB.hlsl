#include "globals.hlsli"
#include "geometryInclude.hlsli"
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
    //float3 worldPos : WPOS;
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
    //output.worldPos = output.position;
    output.position = mul(output.position, g_viewProj);

    return output;
}

Texture2D g_albedo : register(t0);
Texture2D g_roughnessTexture : register(t1);
Texture2D g_metallicTexture : register(t2);
Texture2D g_normalTexture : register(t3);
Texture2D g_opacity : register(t8);
Texture2D g_ao : register(t9);
Texture2D g_translucency : register(t10);

static const float3 basicF0 = float3(0.04, 0.04, 0.04);

//pixel shader

struct PS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float2 roughnessMetalness : SV_Target2;
    float4 emission : SV_Target3;

};

PS_OUTPUT ps_main(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace)
{
    PS_OUTPUT output = (PS_OUTPUT)0;

    output.albedo.rgb = g_albedo.Sample(g_sampler, input.tex);
    //output.albedo.a = g_opacity.Sample(g_sampler, input.tex);
    output.albedo.a = grassAlpha(input.tex, g_opacity);

    if (output.albedo.a < ALPHA_THRESHOLD)
        discard;

    input.normal = isFrontFace ? input.normal : -input.normal;

    output.normal.zw = packOctahedron(input.normal);

    float3 bitangent = float3(0, -1, 0);
    float3 tangent = cross(bitangent, input.normal);

    float3x3 TBN = float3x3(tangent, bitangent, input.normal);

    input.normal = g_normalTexture.Sample(g_sampler, input.tex).xyz * 2.0 - 1.0;
    input.normal.y = -input.normal.y;

    input.normal = normalize(mul(input.normal, TBN));

    output.normal.xy = packOctahedron(input.normal);

    float roughness = g_roughnessTexture.Sample(g_sampler, input.tex);

    float metallic = g_metallicTexture.Sample(g_sampler, input.tex);

    output.roughnessMetalness = float2(roughness, metallic);

    output.emission.rgb = g_translucency.Sample(g_sampler, input.tex);
    
    output.emission.a = g_ao.Sample(g_sampler, input.tex).r;

    return output;
}