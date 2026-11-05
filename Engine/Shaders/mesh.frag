#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 n = normalize(fragNormal) * 0.5 + 0.5;
    outColor = vec4(n, 1.0);
}