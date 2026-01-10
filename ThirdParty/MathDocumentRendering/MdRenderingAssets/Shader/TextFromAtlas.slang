#version 450
layout (location = 0) in VSInput {
    float2 Pos;
    float2 TexCoord;
} input; 

layout (location = 2) in VSInstanceData {
    float2 Pos;
	float2 Size;
	float2 TextureOffset;
} InstanceData; 

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexCoord;
    float2 TextureOffset;
    int2 PixelPos;
};

layout (set = 0, binding = 0) uniform Sampler2D texSampler;
struct UniformBuffer {
    uint2 Extent;
};
layout(set = 0, binding = 1) ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VSOutput vertMain() {
    VSOutput output;
    float2 VertPos = input.Pos;
    VertPos.x *= InstanceData.Size.x;
    VertPos.y *= InstanceData.Size.y;
    VertPos += InstanceData.Pos;
    output.PixelPos = int2(int(InstanceData.Pos.x), int(InstanceData.Pos.y));
    vec2 ndc = (VertPos / ubo.Extent) * 2.0 - 1.0;
    output.pos = float4(ndc,0 ,1);
    output.fragTexCoord = input.TexCoord;
    output.TextureOffset = InstanceData.TextureOffset;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    
    int2 TexelCoord = 
        int2(int(gl_FragCoord.x + vertIn.TextureOffset.x),int(gl_FragCoord.y + vertIn.TextureOffset.y))
        - vertIn.PixelPos;
    float4 Col = texelFetch(texSampler,  TexelCoord);
    if(Col.a < 0.01)
        discard;
    return Col;
}