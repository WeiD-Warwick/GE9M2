Texture2D albedoTex : register(t0);
SamplerState samplerLinear : register(s0);

cbuffer fogCB : register(b0)
{
    float4x4 VP;
    float3 cameraRight;
    float _pad0;
    float3 cameraUp;
    float _pad1;
    float time;
    float fogDensity;
    float fogNoiseScale;
    float fogOpacity;
    float3 fogColor;
    float _pad2;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoords : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

float hash21(float2 p)
{
    // Simple hash-based noise
    p = frac(p * float2(123.34, 345.45));
    p += dot(p, p + 34.345);
    return frac(p.x * p.y);
}

float4 PS(PS_INPUT input) : SV_Target0
{
    float2 uv = input.TexCoords;
    float radial = saturate(1.0f - length(uv - 0.5f) * 1.6f);

    // Animated soft noise
    float n = hash21(uv * (2.0f + fogNoiseScale) + time * 0.1f);
    float softness = lerp(0.55f, 1.0f, n);

    float alpha = radial * softness * fogOpacity;

    float3 color = fogColor * fogDensity;

    return float4(color, alpha);
}