#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUv;

layout(location = 0) out vec3 outColor;

void main()
{
    mat4 view = mat4(
         1,  0,  0, 0,
         0,  1,  0, 0,
         0,  0,  1, 0,
        -1, -1, -3, 1
    );

    mat4 proj = mat4(
        0.733064294, 0, 0, 0,
        0, 1.3032254, 0, 0,
        0, 0, -1.00001001, -1,
        0, 0, -0.100000992, 0
    );

    outColor = inNormal;
    gl_Position = proj * view * vec4(inPosition, 1.0);
}