#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

struct VertexShaderInput
{
    float3 pos : POSITION;
    float2 color : TEXCOORD;
};


//struct PixelShaderInput
//{
//    float4 posProj : SV_POSITION;
//    float3 color : COLOR;
//};

//struct PixelShaderOutput
//{
//    float4 pixelColor : SV_Target;
//};


#endif