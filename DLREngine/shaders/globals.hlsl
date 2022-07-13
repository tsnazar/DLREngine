cbuffer PerFrame : register(b0)
{
    float4x4 g_viewProj;
}

SamplerState g_sampler : SAMPLER: register(s0);

float ZNEAR = 1.0f;
float ZFAR = 0.0f;