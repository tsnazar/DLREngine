#include "gpuParticlesInclude.hlsli"

RWStructuredBuffer<int> g_range : register(u1);
RWBuffer<uint> g_indirectArgs : register(u2);

cbuffer Constants : register(b1)
{
	uint g_bufferSize;
	float g_maxAnimationTime;
	float CPadding1;
	float CPadding2;
};

[numthreads(1, 1, 1)]
void cs_main(uint3 threadID : SV_DispatchThreadID)
{
	int deadCount = g_range[2];
	int count = g_range[0];
	int countOverflow = count > g_bufferSize ? count % g_bufferSize : 0;
	deadCount = deadCount > countOverflow ? deadCount : countOverflow;

	g_range[0] = count > g_bufferSize ? g_bufferSize : count;
	g_range[0] -= deadCount;
	g_range[1] = (g_range[1] + deadCount) % g_bufferSize;
	g_range[2] = 0;
	g_indirectArgs[1] = g_range[0];
}