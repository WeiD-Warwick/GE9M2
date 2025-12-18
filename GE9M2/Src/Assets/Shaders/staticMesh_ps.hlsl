#define MAX_POINT_LIGHTS 10
#define PI 3.1415926

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);

SamplerState samplerLinear : register(s0);

cbuffer staticMeshBuffer : register(b0) {
    float2 uvScale;
    
    int useNormalMap;
    bool useAlphaTest;
    
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

float3x3 getTBN(PS_INPUT input)
{
    float3 N = normalize(input.NormalWS);
    float3 T = normalize(input.TangentWS);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);
    return TBN;
}

float4 PS(PS_INPUT input) : SV_Target0 {
    
    // --- Albedo ---
    float4 albedoSample = albedoTex.Sample(samplerLinear, input.TexCoords * uvScale);

    // --- Alpha Test ---
    if (useAlphaTest && albedoSample.a < 0.5f)
        discard;

    float3 albedo = albedoSample.rgb;
    //return float4(albedo, 1.0);

    // --- TBN ---
    float3 normalWS = normalize(input.NormalWS);
    if (useNormalMap)
    {
        float3x3 TBN = getTBN(input);
    
        float3 mapNormal = normalTex.Sample(samplerLinear, input.TexCoords * uvScale).xyz;
        mapNormal = normalize(mapNormal * 2.0 - 1.0);
        normalWS = normalize(mul(mapNormal, TBN));
    }

    // --- Sky Light ---
    float3 lighting = albedo * skyLightColor * skyLightIntensity;
    
    // --- Point Light ---
    for (int i = 0; i < pointLightCount; ++i) {
        float3 L = lightPosWS[i] - input.PosWS;
        float dist = length(L);
        float3 lightDir = normalize(L);
        
        // attenuation
        float attenuation = saturate(1.0 - dist / lightRange[i]);
        attenuation *= attenuation;
        
        // Lambert
        float NdotL = max(dot(normalWS, lightDir), 0.0);
        float3 diffuse = (albedo / PI) * lightColor[i] * NdotL;

        lighting += diffuse * lightIntensity[i] * attenuation;
    }

    return float4(lighting, 1.0);
}