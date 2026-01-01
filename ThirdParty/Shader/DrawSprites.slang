#version 450
layout (location = 0) in VSInput {
    float2 Pos;
    float2 TexCoord;
} Vert; 

layout (location = 2) in VSInstanceData {
    int2 Pos;
	int2 Size;
	int2 TexPos;
    int2 TexSize; 
} Inst; 

layout (set = 0, binding = 0) uniform Sampler2D Atlas;

struct UniformBuffer {
    uint2 Extent;
};
layout(set = 0, binding = 1) ConstantBuffer<UniformBuffer> Ubo;

[shader("vertex")]
VSOutput vertMain() {
    VSOutput Output;
    float2 VertPos = Vert.Pos;
    VertPos.x *= Inst.Size.x;
    VertPos.y *= Inst.Size.y;
    VertPos += Inst.Pos;
    float2 NormVertPos = (VertPos / Ubo.Extent) * 2.0 - 1.0;
    Output.pos = float4(NormVertPos,0 ,1);
    Output.TexCoord = Vert.TexCoord;
    Output.TexPos = Inst.TexPos;
    Output.TexSize = Inst.TexSize;
    return Output;
}

struct VSOutput
{
    float4 pos : SV_Position;
    float2 TexCoord;
    int2 TexPos;
    int2 TexSize; 
};

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    
    int2 TexelPos = int2(vertIn.TexCoord * vertIn.TexSize) + vertIn.TexPos;
    float4 Color = texelFetch(Atlas, TexelPos);
    if(Color.a < 0.01f)
        discard;
    return Color;
}