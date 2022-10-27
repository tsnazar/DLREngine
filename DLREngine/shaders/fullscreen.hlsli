#ifndef FULLSCREEN
#define FULLSCREEN

struct VS_OUTPUT {
    float4 position : SV_Position;
    float3 dir: SampleDir;
    float2 tex : TEX;
};

VS_OUTPUT vs_main(uint vertexID : SV_VertexID) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.tex = float2((vertexID << 1) & 2, vertexID & 2);
    output.position = float4(output.tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), ZFAR, 1.0f);
    output.dir = g_frustumCorners[0] + output.tex.x * g_frustumCorners[1] + output.tex.y * g_frustumCorners[2];

    return output;
}
#endif