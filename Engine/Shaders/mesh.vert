#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUV;

layout(set = 0, binding = 0) uniform Camera { mat4 view; mat4 projection; vec4 lightPos; } camera;
layout(location = 2) out vec3 fragWorldPos;
layout(push_constant) uniform Push { mat4 model; vec4 params; } push;
void main()
{
    gl_Position = camera.projection * camera.view * push.model * vec4(inPosition, 1.0);
    fragNormal = mat3(push.model) * inNormal;
    fragUV = inUV;
    fragWorldPos = (push.model * vec4(inPosition, 1.0)).xyz;
}