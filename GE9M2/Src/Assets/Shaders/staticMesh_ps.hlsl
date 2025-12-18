#define MAX_POINT_LIGHTS 10
#define PI 3.1415926

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);

SamplerState samplerLinear : register(s0);

cbuffer staticMeshBuffer : register(b0)
{
    float4x4 W;
    float4x4 V;
    float4x4 P;
    
    float2 uvScale;
    int useNormalMap;
    float skyLightIntensity;
    
    float3 skyLightColor;
    int pointLightCount;
    
    float3 lightPosWS[MAX_POINT_LIGHTS];
    float lightRange[MAX_POINT_LIGHTS];
    
    float3 lightColor[MAX_POINT_LIGHTS];
    float lightIntensity[MAX_POINT_LIGHTS];
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 PosWS : TEXCOORD0;
    float3 NormalWS : TEXCOORD1;
    float3 TangentWS : TEXCOORD2;
    float2 TexCoords : TEXCOORD3;
};

// Attenuation
float Attenuation(float d, float range) {
    float x = saturate(1.0 - d / max(range, 1e-4));
    return x * x;
}

float3x3 getTBN(PS_INPUT input)
{
    float3 N = normalize(input.NormalWS);
    float3 T = normalize(input.TangentWS);
    float3 B = normalize(cross(input.NormalWS, T));
    float3x3 TBN = float3x3(T, B, N);
    return TBN;
}

float4 PS(PS_INPUT input) : SV_Target0 {
    
    // --- Albedo ---
    float3 albedo = albedoTex.Sample(samplerLinear, input.TexCoords * uvScale).rgb;
    
    // --- TBN ---
    float3x3 TBN = getTBN(input);
    
    float3 mapNormal = normalTex.Sample(samplerLinear, input.TexCoords * uvScale).xyz;
    mapNormal = normalize(mapNormal * 2.0 - 1.0);

    // --- Sky Light ---
    float3 lighting = albedo * skyLightColor * skyLightIntensity;
    
    // --- Point Light ---
    for (int i = 0; i < pointLightCount; ++i) {
        float dist = length(lightPosWS[i] - input.PosWS);
        float3 lightDir = normalize(lightPosWS[i] - input.PosWS);
        float3 localLightDir = normalize(mul(lightDir, transpose(TBN)));
        
        // attenuation
        float attenuation = saturate(1.0 - dist / lightRange[i]);
        attenuation *= attenuation;
        
        // Lambert
        float diffuse = (albedo / PI) * lightColor[i] * max(dot(mapNormal, localLightDir), 0);

        float finalLight = diffuse * lightIntensity[i] * attenuation;

        lighting += finalLight;
    }

    return float4(albedo * skyLightColor * skyLightIntensity, 1.0);
}