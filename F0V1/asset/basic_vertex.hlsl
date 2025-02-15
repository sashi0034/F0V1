#include "basic.hlsli"

cbuffer cb0 : register(b0)
{
    float4x4 g_mat;
}

PSInput VS(float4 position : POSITION, float2 uv : TEXCOORD)
{
    PSInput result;
    result.position = mul(g_mat, position);
    result.uv = uv;
    return result;
}
