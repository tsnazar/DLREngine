#include "globals.hlsli"
#define IBL
#include "lighting.hlsli"

float computeGrassAngle(float2 instancePos, float2 windDir)
{
    const float MAX_VARIATION = PI;
    float WIND_WAVE_LENGTH = 50;
    float POWER_WAVE_LENGTH = 233;
    const float WIND_OSCILLATION_FREQ = 0.666;
    const float POWER_OSCILLATION_FREQ = 1.0 / 8.0;

    float instanceRandom = frac(instancePos.x * 12345);
    float windCoord = dot(instancePos, windDir);

    float windPhaseVariation = instanceRandom * MAX_VARIATION;
    float windPhaseOffset = windCoord / WIND_WAVE_LENGTH + windPhaseVariation;
    float windOscillation = (sin(windPhaseOffset - WIND_OSCILLATION_FREQ * 2 * PI * g_time) + 1) / 2;
    
    float powerPhaseOffset = windCoord / POWER_WAVE_LENGTH;
    float powerOscillation = (sin(powerPhaseOffset - POWER_OSCILLATION_FREQ * 2 * PI * g_time) + 1) / 2;

    float minAngle = lerp(0.0, 0.3, powerOscillation);
    float maxAngle = lerp(0.1, 1., powerOscillation);
    return lerp(minAngle, maxAngle, windOscillation);
}

struct VS_INPUT {
    uint inId : SV_VertexId;
    float3 inPosition : POS;
    float inScale : SCALE;
    float2x2 inRotation : ROT;
};

struct VS_OUTPUT {
    float2 tex : TEX;
    float4 position : WPOS;
    float3 normal : NORM;
    float scale : SCALE;
    float3 worldOffset : WOFF;
};

static const float NUM_GRASS_SECTIONS = 3;

// vertex shader
VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.tex = float2(input.inId % 2, ((input.inId % 8) >> 1) / NUM_GRASS_SECTIONS);
    //output.position = float4((output.tex.x - 0.5), (1 - output.tex.y), ZFAR, 1);
    output.position = float4((output.tex.x - 0.5), 0, ZFAR, 1);
    output.position.xyz *= input.inScale;
    output.position.xz = mul(output.position.xz, input.inRotation);
    
    output.normal = float3(0, 0, -1);
    output.normal.xz = mul(output.normal.xz, input.inRotation);

    output.scale = input.inScale;
    output.worldOffset = input.inPosition;

    return output;
}

struct GS_OUTPUT
{
    float4 position : SV_Position;
    float3 worldPos : WPOS;
    float2 uv : UV;
    float3 normal : NORM;
};

cbuffer WindRotation : register(b4)
{
    float4 g_windRotation;
    float4 g_windInvRotation;
}

//geometry shader
[maxvertexcount(12)]
void gs_main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    float windAngle = computeGrassAngle(input[0].worldOffset.xz, float2(1, 0)) * PI / 2.0;
    windAngle = max(windAngle, MIN_DOT);

    for (uint blade = 0; blade < 4; ++blade)
    {
        float angle = blade * 45.0 * PI / 180.0;

        float bladeSin = 0;
        float bladeCos = 0;
        sincos(angle, bladeSin, bladeCos);
        float2x2 bladeRotation = { bladeCos, bladeSin, 
                                -bladeSin, bladeCos };

        for (uint i = 0; i < 3; ++i)
        {
            GS_OUTPUT output = (GS_OUTPUT)0;

            output.position = input[i].position;
            output.position.xz = mul(output.position.xz, bladeRotation);

            float waveSin = 0;
            float waveCos = 0;
            sincos(windAngle * (1 - input[i].tex.y), waveSin, waveCos);
            float2x2 waveRotation = { waveCos, waveSin, 
                                     -waveSin, waveCos };
            float2x2 waveRotationInv = { waveCos, -waveSin, 
                                         waveSin, waveCos };

            float R = input[i].scale / windAngle;

            output.position.xz = mul(output.position.xz, float2x2(g_windRotation));

            output.position.x += R;
            output.position.xy = mul(output.position.xy, waveRotation);
            output.position.x -= R;
            output.position.xy = mul(output.position.xy, waveRotationInv);

            output.position.xz = mul(output.position.xz, float2x2(g_windInvRotation));

            output.position.xyz += input[i].worldOffset;
            output.worldPos = output.position;
            output.position = mul(output.position, g_viewProj);

            output.normal = input[i].normal;
            output.normal.xz = mul(output.normal.xz, bladeRotation);

            output.uv = input[i].tex;

            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
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

float4 ps_main(GS_OUTPUT input, bool isFrontFace : SV_IsFrontFace) : SV_TARGET
{
    //return float4(1,1,1,1);

    float shadowMapWidth = 0, shadowMapHeight = 0, shadowMapElements = 0;

    g_shadowMap.GetDimensions(shadowMapWidth, shadowMapHeight, shadowMapElements);

    float3 albedo = g_albedo.Sample(g_sampler, input.uv);
 
    input.normal = isFrontFace ? input.normal : -input.normal;
    
    float3 GN = normalize(input.normal);

    float3 bitangent = float3(0, -1, 0);
    float3 tangent = cross(bitangent, input.normal);

    float3x3 TBN = float3x3(tangent, bitangent, input.normal);

    input.normal = g_normalTexture.Sample(g_sampler, input.uv).xyz * 2.0 - 1.0;
    input.normal.y = -input.normal.y;

    float3 N = normalize(mul(input.normal, TBN));

    float roughness = g_roughnessTexture.Sample(g_sampler, input.uv);

    float metallic = g_metallicTexture.Sample(g_sampler, input.uv);

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

    float4 resultColor = float4(0, 0, 0, 0);

    for (uint i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        float3 L = g_lights[i].position - input.worldPos;
        float3 shadowFragPos = input.worldPos;

        float LdotV = pow(max(dot(normalize(-L), V), 0), 4);

        float dist = length(L);
        dist = max(dist, g_lights[i].radius);
        float lightAngleSin = g_lights[i].radius / dist;
        float angularCos = sqrt(1.0 - lightAngleSin * lightAngleSin);

        float shadow = shadowCalculation(N, L, shadowFragPos, g_lights[i].position, g_shadowMap, g_shadowMapWidth, i);

        resultColor.rgb += g_translucency.Sample(g_sampler, input.uv) * g_lights[i].radiance * (1 - angularCos) * 2 * PI * LdotV * shadow;

        resultColor.rgb += calculatePointLighting(N, GN, V, L, view, g_lights[i].radius, g_lights[i].radiance, material, shadow);
    }

    addEnvironmentDiffuse(resultColor.rgb, N, material);

    //resultColor.xyz += diff * g_ao.Sample(g_sampler, input.uv);
    //resultColor.xyz *= g_ao.Sample(g_sampler, input.uv);

    resultColor.a = g_opacity.Sample(g_sampler, input.uv).r;

    resultColor *= g_ao.Sample(g_sampler, input.uv);
    
    return resultColor;
}