#include "Common.hlsli"

cbuffer VertexConstantBuffer : register(b0)
{
    matrix world;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

float4 main(float3 input : POSITION) : SV_Position
{
    //PixelShaderInput output;
    
    float scale = 0.1;
    float4 pos = float4(input * scale, 1.0f);
    //float4x4 mvp = mul(projection, mul(view, world));
    pos = mul(pos, world);
    pos = mul(pos, view);
    pos = mul(pos, projection);
   
    //output.posProj = pos;
    //output.color = input.color;
    
    return pos;
}