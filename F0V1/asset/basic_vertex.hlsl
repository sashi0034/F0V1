#include "basic.hlsli"

PSInput VS(float4 position : POSITION, float2 uv : TEXCOORD)
{
    PSInput result;
    result.position = position;
    result.uv = uv;
    return result;
}
