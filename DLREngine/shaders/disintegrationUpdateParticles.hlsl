#include "globals.hlsli"
#include "gpuParticlesInclude.hlsli"
#include "geometryInclude.hlsli"
cbuffer Constants : register(b1)
{
	uint g_bufferSize;
	float g_maxAnimationTime;
	float CPadding1;
	float CPadding2;
};
RWStructuredBuffer<Particle> g_gpuParticles : register(u0);
RWStructuredBuffer<int> g_range : register(u1);

Texture2D g_depth : register(t0);
Texture2D g_normals : register(t1);

cbuffer RenderDimensions : register(b2)
{
	uint g_targetWidth;
	uint g_targetHeight;
	float2 g_RDpadding;
}

static const float GRAVITY = 0.48;
static const float SPEED_DECREMENT = 0.5;

[numthreads(64,1,1)]
void cs_main(uint3 threadID : SV_DispatchThreadID)
{
	uint count = g_range[0] > g_bufferSize ? g_bufferSize : g_range[0];
	uint firstIndex = g_range[1];
	uint lastIndex = (firstIndex + count) % g_bufferSize;

	bool threadOutOfBounds;

	if (firstIndex > lastIndex)
	{
		threadOutOfBounds = threadID.x >= lastIndex && threadID.x < firstIndex;
	}
	else
	{
		threadOutOfBounds = threadID.x < firstIndex || threadID.x >= lastIndex;
	}

	if ((threadOutOfBounds && count != g_bufferSize) || count == 0)
		return;

	float4 posWS = float4(g_gpuParticles[threadID.x].pos, 1);
	float3 vel = g_gpuParticles[threadID.x].vel;
	float spawnTime = g_gpuParticles[threadID.x].spawnTime;

	float4 posVS = mul(posWS, g_view);
	
	float4 posCS = mul(posVS, g_proj);

	posCS /= posCS.w;

	float2 screenCoords = float2((0.5 * posCS.x + 0.5) * g_targetWidth, (1 - (0.5 * posCS.y + 0.5)) * g_targetHeight);

	float4 normalCompressed = g_normals.Load(int3(screenCoords, 0));
	float3 GN = unpackOctahedron(normalCompressed.zw);

	float depth = g_depth.Load(int3(screenCoords, 0));
	float3 viewPos = viewSpacePositionFromDepth(depth, float2((posCS.x + 1) / 2, (1 - (posCS.y + 1) / 2)));

	vel.y -= GRAVITY * g_deltaTime;

	if (viewPos.z < posVS.z && viewPos.z > (posVS.z - 0.1))
	{
		float4 offset = float4(viewPos - posVS.xyz, 0);
		offset = mul(offset, g_invView);
		g_gpuParticles[threadID.x].pos += offset;
		vel = reflect(vel, GN) * SPEED_DECREMENT;
	}

	g_gpuParticles[threadID.x].pos += vel * g_deltaTime;
	g_gpuParticles[threadID.x].vel = vel;

	if (g_time - spawnTime > g_maxAnimationTime)
	{
		InterlockedAdd(g_range[2], 1);
	}
}