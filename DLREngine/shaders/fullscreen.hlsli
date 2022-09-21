#ifndef FULLSCREEN
#define FULLSCREEN
struct VSQuadOut {
    float4 position : SV_Position;
    float3 dir: SampleDir;
};

VSQuadOut vs_main(uint vertexID : SV_VertexID) {
    VSQuadOut result;

    float2 uv = float2((vertexID << 1) & 2, vertexID & 2);
    result.position = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), ZFAR, 1.0f);
    result.dir = g_frustumCorners[0] + uv.x * g_frustumCorners[1] + uv.y * g_frustumCorners[2];

    return result;
}
#endif