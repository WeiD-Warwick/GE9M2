TextureCube SkyTex : register(t0);
SamplerState SkySampler : register(s0);

struct PS_INPUT
{
    float3 Dir : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target
{
    float3 dir = normalize(input.Dir);

    // dir.z = -dir.z;

    return SkyTex.Sample(SkySampler, dir);
}
