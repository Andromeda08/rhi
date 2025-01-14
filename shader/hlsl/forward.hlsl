struct VSInput {
    float3 position   : POSITION0;
    float3 normal     : NORMAL0;
    float2 uv         : TEXCOORD0;
};

struct VSOutput {
    float4 position   : SV_Position;
    float4 color      : COLOR;
};

VSOutput vertexMain(VSInput input) {
    VSOutput output;

    // HLSL matrices are **row-major**, so CPU-side will need a glm::transpose() for D3D12...
    float4x4 view = float4x4(
        1, 0, 0, -1,
        0, 1, 0, -1,
        0, 0, 1, -3,
        0, 0, 0,  1
    );

    float4x4 proj = float4x4(
        0.733064294, 0,          0,           0,
        0,           1.3032254,  0,           0,
        0,           0,         -1.00001001, -0.100000992,
        0,           0,         -1,           0
    );

    output.position = mul(proj, mul(view, float4(input.position, 1.0f)));
    output.color = float4(input.normal, 1.0f);

    return output;
}

struct PSInput {
    float4 position   : SV_Position;
    float4 color      : COLOR;
};

float4 pixelMain(PSInput input) : SV_Target {
    return input.color;
}