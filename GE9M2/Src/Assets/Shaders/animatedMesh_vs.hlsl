cbuffer animatedMeshBuffer : register(b0) {
    float4x4 M;
    float4x4 V;
    float4x4 P;
    float4x4 bones[256];
};

struct VS_INPUT {
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
    uint4 BoneIDs : BONEIDS;
    float4 BoneWeights : BONEWEIGHTS;
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;

    // Compute the skinning transform
    float4x4 transform = bones[input.BoneIDs[0]] * input.BoneWeights[0];
    transform += bones[input.BoneIDs[1]] * input.BoneWeights[1];
    transform += bones[input.BoneIDs[2]] * input.BoneWeights[2];
    transform += bones[input.BoneIDs[3]] * input.BoneWeights[3];

    float4 pos = mul(transform, float4(input.Pos, 1.0f));
    float4 posM = mul(M, float4(input.Pos, 1));
    float4 posMV = mul(V, posM);
    float4 posMVP = mul(P, posMV);
    
    float3x3 transform3x3 = (float3x3) transform;
    float3x3 M3x3 = (float3x3) M;
    
    float3 normal = mul(transform3x3, input.Normal);
    normal = mul(M3x3, normal);
    
    float3 tangent = mul(transform3x3, input.Tangent);
    tangent = mul(M3x3, tangent);
    
    output.Pos = posMVP;
    output.Normal = normalize(normal);
    output.Tangent = normalize(tangent);
    output.TexCoords = input.TexCoords;
    return output;
}
