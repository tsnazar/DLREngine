#include "globals.hlsli"
#include "geometryInclude.hlsli"

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

//static const float g_maxAnimationTime = 10.0f;

struct VS_INPUT {
    float3 inPosition : POS;
    float2 inTexCoord : TEXCOORD;
    float4x4 inModelToWorld : MAT;
    float3 inSpherePos : SPHEREPOS;
    float inSpawnTime : TIME;
    float inMaxRadius : RADIUS;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 texCoord : TEX;
    float animationTime : TIME;
    float vertexCollisionTime : VTIME;
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

    output.texCoord = input.inTexCoord;

    return output;
}

struct GS_OUTPUT
{
    float4 position : SV_Position;
    nointerpolation uint slice : SV_RenderTargetArrayIndex; 
    float2 texCoord : TEX;
    float animationTime : TIME;
    float vertexCollisionTime : VTIME;
};

cbuffer ShadowTransforms : register(b1)
{
    float4x4 g_shadowTransforms[6];
    uint g_sliceOffset;
    float3 padding;
}

//geometry shader
[maxvertexcount(18)]
void gs_main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    [unroll] for (uint face = 0; face < 6; ++face)
    {
        [unroll] for (uint i = 0; i < 3; ++i)
        {
            GS_OUTPUT output = (GS_OUTPUT)0;
            output.slice = g_sliceOffset + face;
            output.position = mul(input[i].position, g_shadowTransforms[face]);

            output.animationTime = input[i].animationTime;
            output.vertexCollisionTime = input[i].vertexCollisionTime;
            output.texCoord = input[i].texCoord;

            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

static const float EDGE_DELTA = 0.05;
static const float DISSOLVE_DELTA = 0.2;

Texture2D g_noiseTexture : register(t8);

///pixel shader
void ps_main(GS_OUTPUT input)
{
    float dissolveNoise = g_noiseTexture.Sample(g_sampler, input.texCoord);

    float dissolveAnimationTime = (input.animationTime - input.vertexCollisionTime - EDGE_DELTA) / DISSOLVE_DELTA;

    if (input.animationTime > (input.vertexCollisionTime + EDGE_DELTA) && dissolveAnimationTime >= dissolveNoise)
        discard;
}