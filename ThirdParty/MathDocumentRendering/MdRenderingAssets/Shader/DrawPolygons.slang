#version 450
layout (location = 0) in VSInput {
    float2 Pos;
    float4 Color;
} Vert; 

struct UniformBuffer {
    uint3 Extent;
};

layout(set = 0, binding = 0) ConstantBuffer<UniformBuffer> Ubo;

struct VSOutput
{
    float4 pos : SV_Position;
    float4 Color;
};

[shader("vertex")]
VSOutput vertMain() {
    VSOutput Output;
    float2 VertPos = Vert.Pos;
    float2 NormVertPos = (VertPos / Ubo.Extent.xy) * 2.0 - 1.0;
    Output.pos = float4(NormVertPos,0 ,1);
    Output.Color = Vert.Color;
    return Output;
}


[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return vertIn.Color;
}