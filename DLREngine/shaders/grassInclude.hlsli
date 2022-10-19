#ifndef GRASS_FUNCTIONS
#define GRASS_FUNCTIONS
#include "globals.hlsli"

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

float grassAlpha(float2 tc, Texture2D alphaTexture)
{
    float alpha = alphaTexture.Sample(g_grassSampler, tc);

    const int FADING_PIXELS = 1;
    const float THRESHOLD = 0.1;
    float deltaAlpha = min(1.0, abs(ddx(alpha)) + abs(ddx(alpha)));
    float edgeDistance = (alpha - THRESHOLD) / deltaAlpha;
    alpha = saturate(edgeDistance / FADING_PIXELS);

    return alpha;
}

#endif