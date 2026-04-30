struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VSOutput main(VSInput IN)
{
    VSOutput OUT;
    OUT.position = float4(IN.position, 1.0);
    OUT.color = IN.color;
    return OUT;
}