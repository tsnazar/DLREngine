#include "globals.hlsli"
#define IBL
#include "lighting.hlsli"

struct VS_INPUT {
    uint inId : SV_VertexId;
    float3 inPos : POS;
    float inFrameFracTime : TIME;
    float4 inTint : TINT;
    float2x2 inRotation : ROT;
    float3 inSize : SIZE;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 tex : TCOORD;
    float time : FRAMETIME;
    float thickness : THICKNESS;
    float4 color : COLOR;
    float2x2 rot : ROT;
    float3 viewPos : VPOS;
    float depth : DEPTH;
    float3 worldPos : WPOS;
    float3 worldNormal : WNORM;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.tex = float2(input.inId % 2, (input.inId % 4) >> 1);
    output.position = float4((output.tex.x - 0.5) * 2, -(output.tex.y - 0.5) * 2, 0, 1);
    output.position.x *= input.inSize.x;
    output.position.y *= input.inSize.y;
    output.position.xy = mul(output.position.xy, input.inRotation);
    output.position.xyz += mul(float4(input.inPos, 1), g_view);
    output.viewPos = output.position.xyz;
    output.worldPos = mul(output.position, g_invView);
    output.worldNormal = normalize(mul(float4(0, 0, -1, 0), g_invView));
    output.position = mul(output.position, g_proj);
    output.depth = output.position.z / output.position.w;

    output.rot = input.inRotation;

    output.color = input.inTint;
    output.time = 1.0 - input.inFrameFracTime;

    output.thickness = input.inSize.z;

    return output;
}

Texture2D g_smokeEMVA : register(t0);
Texture2D g_smokeRLT : register(t1);
Texture2D g_smokeBotBF : register(t2);
Texture2D g_depth : register(t3);
TextureCubeArray g_shadowMap : register(t4);

static const float g_mvScale = 0.0001;

static const float3 basicF0 = float3(0.04, 0.04, 0.04);

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    input.tex /= 8;

    uint thisFrame = lerp(0, 62, input.time);
    uint nextFrame = thisFrame + 1;

    uint tfXOff = thisFrame % 8;
    uint tfYOff = (thisFrame - tfXOff) / 8;

    uint nfXOff = nextFrame % 8;
    uint nfYOff = (nextFrame - nfXOff) / 8;

    float2 uvThis = float2(input.tex.x + tfXOff / 8.0, input.tex.y + tfYOff / 8.0);
    float2 uvNext = float2(input.tex.x + nfXOff / 8.0, input.tex.y + nfYOff / 8.0);

    float2 mvA = 2.0 * g_smokeEMVA.Sample(g_sampler, uvThis).gb / 255.0 - 1.0;
    float2 mvB = 2.0 * g_smokeEMVA.Sample(g_sampler, uvNext).gb / 255.0 - 1.0;

    float2 uvA = uvThis;
    uvA -= mvA * g_mvScale * input.time;

    float2 uvB = uvNext;
    uvB -= mvB * g_mvScale * (input.time - 1.f);

    float valueA = g_smokeEMVA.Sample(g_sampler, uvA).a;
    float valueB = g_smokeEMVA.Sample(g_sampler, uvB).a;

    float value = lerp(valueA, valueB, input.time);
    
    float4 result = { 0, 0, 0, input.color.w * value };

    float3 xAxis = float3(input.rot[0], 0);
    float3 yAxis = float3(input.rot[1], 0);

    for (uint i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        float3 lightViewPos = mul(float4(g_lights[i].position, 1), g_view);
        float3 L = lightViewPos - input.viewPos;

        float dist = length(L);
        dist = max(dist, g_lights[i].radius);
        float lightAngleSin = g_lights[i].radius / dist;
        float angularCos = sqrt(1.0 - lightAngleSin * lightAngleSin);

        L = normalize(L);

        float XdotL = dot(xAxis, L);
        float YdotL = dot(yAxis, L);
        float ZdotL = dot(float3(0, 0, -1), L);

        float x = max(XdotL , 0) * g_smokeRLT.Sample(g_sampler, uvA).r;
        float mx = max(-XdotL, 0) * g_smokeRLT.Sample(g_sampler, uvA).g;
        float y = max(YdotL, 0) * g_smokeRLT.Sample(g_sampler, uvA).b;
        float my = max(-YdotL, 0) * g_smokeBotBF.Sample(g_sampler, uvA).r;
        float z = max(ZdotL, 0) * g_smokeBotBF.Sample(g_sampler, uvA).b;
        float mz = max(-ZdotL, 0) * g_smokeBotBF.Sample(g_sampler, uvA).g;

        float x1 = max(XdotL, 0) * g_smokeRLT.Sample(g_sampler, uvB).r;
        float mx1 = max(-XdotL, 0) * g_smokeRLT.Sample(g_sampler, uvB).g;
        float y1 = max(YdotL, 0) * g_smokeRLT.Sample(g_sampler, uvB).b;
        float my1 = max(-YdotL, 0) * g_smokeBotBF.Sample(g_sampler, uvB).r;
        float z1 = max(ZdotL, 0) * g_smokeBotBF.Sample(g_sampler, uvB).b;
        float mz1 = max(-ZdotL, 0) * g_smokeBotBF.Sample(g_sampler, uvB).g;

        float coef = min(x + mx + y + my + mz + z, 1.0);
        float coef1 = min(x1 + mx1 + y1 + my1 + mz1 + z1, 1.0);

        coef = lerp(coef, coef1, input.time);

        float shadow = shadowCalculation(input.worldNormal, normalize(g_lights[i].position - input.worldPos), input.worldPos, g_lights[i].position, g_shadowMap, g_shadowMapWidth, i);

        result.xyz += input.color.xyz * coef * g_lights[i].radiance * (1 - angularCos) * 2 * PI * shadow;
    }

    Material material;
    material.albedo = input.color.xyz;
    material.f0 = basicF0;
    material.roughness = 0.7;
    material.metallic = 0.0;

    addEnvironmentDiffuse(result.rgb, input.worldNormal, material);

    float sceneDepth = g_depth.Load(int3(input.position.xy, 0));

    float particleFading = saturate((sceneDepth - input.depth) / input.thickness);

    result.a *= (1 - particleFading);

    return result;
}