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

struct VS_INPUT {
    float3 inPosition : POS;
    float3 inNormal : NORMAL;
    float4x4 inModelToWorld : MAT;
    float3 inSpherePos : SPHEREPOS;
    float inSpawnTime : TIME;
    float inMaxRadius : RADIUS;
};

struct VS_OUTPUT
{
    float3 pos : POS;
    float vertexCollisionTime : TIME;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float4 position = float4(input.inPosition, 1.0);
    position = mul(position, meshToModel);

    float sphereToVertexDist = length(position.xyz - input.inSpherePos);

    output.pos = mul(position, input.inModelToWorld);
    output.vertexCollisionTime = sphereToVertexDist / input.inMaxRadius * g_maxAnimationTime + input.inSpawnTime;

    return output;
}

RWStructuredBuffer<Particle> g_gpuParticles : register(u0);
RWStructuredBuffer<int> g_range : register(u1);

[maxvertexcount(1)]
void gs_main(triangle VS_OUTPUT input[3])
{
    //float triangleCollisionTime = (input[0].vertexCollisionTime + input[1].vertexCollisionTime + input[2].vertexCollisionTime) / 3;
    //float triangleCollisionTime = (input[0].vertexCollisionTime);
    float triangleCollisionTime = max(input[0].vertexCollisionTime, max(input[1].vertexCollisionTime,input[2].vertexCollisionTime));

    if (triangleCollisionTime < g_time && triangleCollisionTime > g_time - g_deltaTime)
    {
        float3 triangleCenter = (input[0].pos + input[1].pos + input[2].pos) / 3;

        float3 AB = input[1].pos - input[0].pos;
        float3 AC = input[2].pos - input[0].pos;

        float3 triangleNormal = normalize(cross(AB, AC));

        Particle particle = (Particle)0;
        particle.pos = triangleCenter;
        particle.spawnTime = triangleCollisionTime;
        particle.vel = triangleNormal;
        uint index = 0;
        InterlockedAdd(g_range[0], 1, index);
        g_gpuParticles[(g_range[1] + index) % g_bufferSize] = particle;
    }
}

//void vs_main(VS_INPUT input)
//{
//    float4 position = float4(input.inPosition, 1.0);
//    position = mul(position, meshToModel);
//
//    float sphereToVertexDist = length(position.xyz - input.inSpherePos);
//    float animationTime = (g_time - input.inSpawnTime) / g_maxAnimationTime;
//    float radius = animationTime * input.inMaxRadius;
//
//    float particleSpawnTime = sphereToVertexDist / input.inMaxRadius * g_maxAnimationTime + input.inSpawnTime;
//
//    float radiusDelta = sphereToVertexDist - radius;
//
//    if (particleSpawnTime < g_time && particleSpawnTime > g_time - g_deltaTime)
//    {
//        Particle particle = (Particle)0;
//        particle.pos = mul(position, input.inModelToWorld);
//        particle.spawnTime = g_time;
//        particle.vel = input.inNormal;
//        uint index = 0;
//        InterlockedAdd(g_range[0], 1, index);
//        g_gpuParticles[(g_range[1] + index) % g_bufferSize] = particle;
//    }
//}