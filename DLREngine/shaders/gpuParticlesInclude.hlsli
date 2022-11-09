#ifndef GPU_PARTICLES
#define GPU_PARTICLES
struct Particle
{
    float3 pos;
    float spawnTime;
    float3 vel;
    float Ppadding;
};
#endif