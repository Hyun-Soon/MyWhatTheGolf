#include "Common.hlsli"

//cbuffer PixelConstantBuffer
//{
    
//};

float4 main(PixelShaderInput input) : SV_Target
{
    return float4(input.color, 1);
}