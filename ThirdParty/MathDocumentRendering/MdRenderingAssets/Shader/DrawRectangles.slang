#version 450
layout (location = 0) in VSInput {
    float2 Pos;
} Vert; 

layout (location = 1) in VSInstanceData {
    int2 Pos;
	int2 Size;
    float4 Color;
} Inst; 

struct UniformBuffer {
    uint2 Extent;
};

layout(set = 0, binding = 0) ConstantBuffer<UniformBuffer> Ubo;

[shader("vertex")]
VSOutput vertMain() {
    VSOutput Output;
    float2 VertPos = Vert.Pos;
    VertPos.x *= Inst.Size.x;
    VertPos.y *= Inst.Size.y;
    VertPos += Inst.Pos;
    float2 NormVertPos = (VertPos / Ubo.Extent) * 2.0 - 1.0;
    Output.pos = float4(NormVertPos,0 ,1);
    Output.Color = Inst.Color;
    return Output;
}

struct VSOutput
{
    float4 pos : SV_Position;
    float4 Color;
};

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return vertIn.Color;
}