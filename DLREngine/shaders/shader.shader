/* vertex attributes go here to input to the vertex shader */
struct vs_in {
    float3 position_local : POS;
    float4 color_local : COL;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out {
    float4 position_clip : SV_POSITION; // required output of VS
    float4 color_local : COLOR;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out)0; // zero the memory first
    output.position_clip = float4(input.position_local, 1.0);
    output.color_local = input.color_local;
    
    return output;
}

float4 ps_main(vs_out input) : SV_TARGET{
  return input.color_local; // must return an RGBA colour
}