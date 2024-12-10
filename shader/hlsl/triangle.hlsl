struct VSInput {
    uint vertexID : SV_VertexID;
};

struct VSOutput {
    float4 position : SV_Position;
    float4 color    : COLOR;
};

VSOutput vertexMain(VSInput input) {
    float3 positions[3] = {
        float3( 0.0f,  0.5f, 0.0f),
        float3( 0.5f, -0.5f, 0.0f),
        float3(-0.5f, -0.5f, 0.0f)
    };

    float4 colors[3] = {
        float4(1.0f, 0.0f, 0.0f, 1.0f),
        float4(0.0f, 1.0f, 0.0f, 1.0f),
        float4(0.0f, 0.0f, 1.0f, 1.0f)
    };

    VSOutput output;
    output.position = float4(positions[input.vertexID], 1.0f);
    output.color = colors[input.vertexID];

    return output;
}

struct PSInput {
    float4 position : SV_Position;
    float4 color    : COLOR;
};

float4 pixelMain(PSInput input) : SV_Target {
    return input.color;
}