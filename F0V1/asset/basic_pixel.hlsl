#include "basic.hlsli"

float4 PS(PSInput input) : SV_TARGET
{
    return float4(input.uv, 1, 1);
}
