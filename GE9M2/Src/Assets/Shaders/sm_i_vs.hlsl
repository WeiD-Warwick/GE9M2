#define MAX_POINT_LIGHTS 10

cbuffer smCB : register(b0) {
    float4x4 VP;
    float time;
    int useVSAnim;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
    
    float4x4 World : WORLD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 PosWS : TEXCOORD0;
    float3 NormalWS : TEXCOORD1;
    float3 TangentWS : TEXCOORD2;
    float2 TexCoords : TEXCOORD3;
};

PS_INPUT VS(VS_INPUT input)
{
    float3 pos = input.Pos;
    
    // Vertex animation
    if (useVSAnim == 1)
    {

        float height = saturate(pos.y * 5.0f);
        float windSpeed = 1.5f;
        float windStrength = 0.3;
    
        float t = time.x * windSpeed;
   
        float swayX = sin(t + pos.y) * windStrength * height;
        float swayZ = cos(t + pos.y) * windStrength * height;

        pos.x += swayX;
        pos.z += swayZ;
    }

    PS_INPUT output;
    // --- Position ---
    float4 worldPos = mul(float4(pos, 1.0f), input.World);
    output.PosWS = worldPos;
    output.Pos = mul(worldPos, VP);
    
    // --- NormalWS ---
    output.NormalWS = normalize(mul(input.Normal, (float3x3) input.World));
    
    // --- TangentWS ---
    output.TangentWS = normalize(mul(input.Tangent, (float3x3) input.World));
    
    // --- TexCoords ---
    output.TexCoords = input.TexCoords;

    return output;
}