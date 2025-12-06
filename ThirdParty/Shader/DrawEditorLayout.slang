#version 450
layout (location = 0) in VSInput {
    float3 inPosition;
    float2 inTexCoord;
} input; 

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexCoord;
};

layout (set = 0, binding = 0) uniform Sampler2D texSampler;

[shader("vertex")]
VSOutput vertMain() {
    VSOutput output;
    output.pos = float4(input.inPosition.xyz ,1);
    output.fragTexCoord = input.inTexCoord;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return texture(texSampler, vertIn.fragTexCoord);
}