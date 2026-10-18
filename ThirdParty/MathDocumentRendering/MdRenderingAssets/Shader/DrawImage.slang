#version 450
layout (location = 0) in VSInput {
    float2 Pos;
    float2 TexCoord;
} Vert; 

layout (set = 0, binding = 0) uniform Sampler2D Atlas;

struct VSOutput
{
    float4 pos : SV_Position;
    float2 TexCoord;
};

[shader("vertex")]
VSOutput vertMain() {
    VSOutput Output;
    Output.pos = float4(Vert.Pos, 1,1);
    Output.TexCoord = Vert.TexCoord;

    return Output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    
    float4 Color = Atlas.Sample(vertIn.TexCoord);
    if(Color.a < 0.01f)
        discard;
    return Color;
}