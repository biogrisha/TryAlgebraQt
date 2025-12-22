#version 450
layout (location = 0) in VSInput {
    float2 inPosition;
} input; 

layout (location = 1) in VSInstanceData {
    float2 Offset;
    float2 Size;
	uint StartIndex;
	uint CurvesCount;
} InstanceData; 

struct VSOutput
{
    float4 pos : SV_Position;
    float2 Offset;
	uint StartIndex;
	uint CurvesCount;
};

struct CurvePoints {
    float2[3] points;
    float a;
    float b;
};
layout(binding = 0) StructuredBuffer<CurvePoints> curves;

struct UniformBuffer {
    uint2 Extent;
};
layout(binding = 1) ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VSOutput vertMain() {
    VSOutput output;
    float2 VertPos = input.inPosition;
    VertPos.x *= InstanceData.Size.x;
    VertPos.y *= InstanceData.Size.y;
    VertPos += InstanceData.Offset;
    vec2 ndc = (VertPos / ubo.Extent) * 2.0 - 1.0;
    output.pos = float4(ndc,0 ,1);
    output.Offset = InstanceData.Offset;
    output.StartIndex = InstanceData.StartIndex;
    output.CurvesCount = InstanceData.CurvesCount;
    return output;
}



float GetVal(float A, float B, float C, float t)
{
    return (1.f - t)*(1.f - t)*A + 2.f*(1.f - t)*t*B + t*t*C;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
 
    int count = 0;
    float Antialiasing = 0.f;
    for(int i = vertIn.StartIndex; i < vertIn.StartIndex + vertIn.CurvesCount; i++)
    {
        float2 Point1 = curves[i].points[0] - gl_FragCoord.xy * 64 + vertIn.Offset * 64;
        float2 Point2 = curves[i].points[1] - gl_FragCoord.xy * 64 + vertIn.Offset * 64;
        float2 Point3 = curves[i].points[2] - gl_FragCoord.xy * 64 + vertIn.Offset * 64;

        float x1 = Point1.x;
        float x2 = Point2.x;
        float x3 = Point3.x;
        float y1 = Point1.y;
        float y2 = Point2.y;
        float y3 = Point3.y;

        float a = curves[i].a;
        float b = curves[i].b;
        float c = y1;
        float t1 = -1.f;
        float t2 = -1.f;

        if(abs(a) < 0.001)
        {
            t1 = t2 = c/(2.f*b);
        }
        else
        {
            float discr = max(b*b - a*c, 0);
            t1 = (b - sqrt(discr))/(a);
            t2 = (b + sqrt(discr))/(a);
        }

        uint code = (0x2E74U >> (((y1 > 0.0) ? 2U : 0U) +
		            ((y2 > 0.0) ? 4U : 0U) + ((y3 > 0.0) ? 8U : 0U))) & 3U;
        {
            float P1 = GetVal(x1, x2, x3, t1);
            if(P1 >= 0)
            {
                if (((code >> 0 ) & 1) > 0) count += 1;
            }
        }
        {
            float P2 = GetVal(x1, x2, x3, t2);
            if(P2 >= 0)
            {
			    if (((code >> 1) & 1) > 0) count -= 1;
            }
        }
    }
    float color = count;
    return float4(color, color,float(vertIn.CurvesCount) / 50.f,1);
}