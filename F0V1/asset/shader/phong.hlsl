Texture2D<float4> g_texture0 : register(t0);

SamplerState g_sampler0 : register(s0);

cbuffer SceneState : register(b0)
{
    float4x4 g_worldMat;
    float4x4 g_viewMat;
    float4x4 g_projectionMat;
}

cbuffer ModelMaterial : register(b1)
{
    float3 g_ambient;
    float3 g_diffuse;
    float3 g_specular;
    float g_shininess;
}

cbuffer DirectionLight : register(b2)
{
    float3 g_lightColor;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

PSInput VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    PSInput result;

    result.position = mul(g_worldMat, position);
    result.position = mul(g_viewMat, result.position);
    result.position = mul(g_projectionMat, result.position);

    result.color = g_diffuse;

    result.uv = uv;
    return result;
}

float4 PS(PSInput input) : SV_TARGET
{
    return float4(g_lightColor, 1.0f);
    // TODO

    const float z = input.position.z;

    const float3 texColor = g_texture0.Sample(g_sampler0, input.uv);
    // const float3 texColor = float3(input.uv.x, input.uv.y, 0);

    return float4(texColor * input.color * z, 1.0);
}
