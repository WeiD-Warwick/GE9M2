cbuffer SkyboxCB : register(b0)
{
    float4x4 V;
    float4x4 P;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Dir : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 pos = float4(input.Pos, 1.0f);

    // View no translation
    float4x4 Vn = V;
    Vn._41 = 0.0f;
    Vn._42 = 0.0f;
    Vn._43 = 0.0f;

    pos = mul(pos, Vn);
    pos = mul(pos, P);

    pos.z = pos.w;
    output.Pos = pos;
    output.Dir = mul(input.Pos, (float3x3) Vn);
    return output;
}
