#version 450
layout (location = 0) in VSInput {
    vec2 inPosition;
    vec2 inTexCoord;
} input; 

struct VSOutput
{
    vec4 pos : SV_Position;
    vec2 fragTexCoord;
};

//layout (set = 0, binding = 0) uniform Sampler2D texSampler;

[shader("vertex")]
VSOutput vertMain() {
    VSOutput output;
    output.pos = float4(input.inPosition.xy,0,1);
    output.fragTexCoord = input.inTexCoord;
    return output;
}

layout (set = 0, binding = 0) uniform Sampler2D texSampler;

[shader("fragment")]
vec4 fragMain(VSOutput vertIn) : SV_TARGET {
    return texture(texSampler, vertIn.fragTexCoord);
    //return vec4(vertIn.fragTexCoord, 1,1);
}