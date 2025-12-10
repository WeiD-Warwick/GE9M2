
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_Target0
{
    float3 normal = normalize(input.Normal);
    
    float3 color = normal * 0.5f + 0.5f;
    return float4(color, 1.0f);
}