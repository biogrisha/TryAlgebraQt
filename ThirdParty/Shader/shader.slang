#version 450
layout (location = 0) in VSInput {
    float3 inPosition;
    float3 inColor;
    float2 inTexCoord;
} input; 

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color;
    float2 fragTexCoord;
};

struct UniformBuffer {
    int Index;
};

struct CurvePoints {
    float2[3] points;
     float a;
     float b;
};

layout (set = 0, binding = 0) ConstantBuffer<UniformBuffer> ubo;
layout(set = 0, binding = 1) StructuredBuffer<CurvePoints> curves;

[shader("vertex")]
VSOutput vertMain() {
    VSOutput output;
    output.pos = float4(input.inPosition,1);
    output.color = input.inColor;
    output.fragTexCoord = input.inTexCoord;
    return output;
}

float GetVal(float A, float B, float C, float t)
{
    return (1.f - t)*(1.f - t)*A + 2.f*(1.f - t)*t*B + t*t*C;
}

float DistToLine(float2 A, float2 B, float2 Point)
{
    A -= Point;
    B -= Point;
    if(dot(A,A - B) < 0 )
    {
        return length(A);
    }
    if(dot(B,B-A) < 0)
    {
        return length(B);
    }

    float area = abs(A.x * B.y - A.y * B.x);
    return area / distance(A, B);
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
 
    int count = 0;
    float Antialiasing = 0.f;
    for(int i = 0; i < ubo.Index; i++)
    {
        float2 Point1 = curves[i].points[0] + float2(0,30)- gl_FragCoord.xy + float2(30,30);
        float2 Point2 = curves[i].points[1] + float2(0,30)- gl_FragCoord.xy + float2(30,30);
        float2 Point3 = curves[i].points[2] + float2(0,30)- gl_FragCoord.xy + float2(30,30);

        float x1 = Point1.x;
        float x2 = Point2.x;
        float x3 = Point3.x;
        float y1 = Point1.y;
        float y2 = Point2.y;
        float y3 = Point3.y;

        float a = y1 - 2.f*y2 + y3;
        float b = y1 - y2;
        float c = y1;
        float t1 = -1.f;
        float t2 = -1.f;

        if(abs(a) < 0.001)
        {
            t1 = t2 = c/(2.f*b);
        }
        else
        {
            float discr = b*b - a*c;
            if(discr < 0) continue;
            t1 = (b + sqrt(discr))/(a);
            t2 = (b - sqrt(discr))/(a);
        }
        float divisor = 0.1f;
        float dist = 1.f;
        if(t1 >= 0 && t1 < 1)
        {
            float P1 = GetVal(x1, x2, x3, t1);
            count += (P1 > 0);
            Antialiasing += (abs(P1) <= dist) ? divisor/pow(abs(P1),2) : 0.f;
        }
        if(t2 >= 0 && t2 < 1 && abs(t2 - t1) > 0.01f)
        {
            float P2 = GetVal(x1, x2, x3, t2);
            count += (P2 > 0);
            Antialiasing += (abs(P2) <= dist) ? divisor/pow(abs(P2),2) : 0.f;
        }
    }
    count = count % 2;
    float color = count ;// + Antialiasing;
    return /*float4(R,G,B,1.0) + float4(value,0,0,1.0) + */float4(color, color,color,1.0);
}