#version 450

layout(set = 0, binding = 0) uniform Camera{ mat4 view; mat4 projection; vec4 lightPos; } camera;

layout(push_constant) uniform Push { mat4 model; vec4 params; } push;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragWorldPos;
layout(location = 0) out vec4 outColor;

void main()
{
    if (push.params.w > 0.5)
    {
        outColor = vec4(push.params.xyz, 1.0);
        return;
    }
    vec3 n = normalize(fragNormal);
    
    vec3 toLight = camera.lightPos.xyz - fragWorldPos;
    float dist = length(toLight);
    vec3 l = toLight / dist;

    float ndl = dot(n, l);
    float wrap = 0.5;
    float diffuse = max((ndl + wrap) / (1.0 + wrap), 0.0);

    float attenuation = 1.0 / (1.0 + 0.15 * dist * dist);
    vec3 ambient = mix(vec3(0.03, 0.03, 0.05), vec3(0.15, 0.15, 0.20), n.y * 0.5 + 0.5);

    vec3 albedo = vec3(0.8, 0.35, 0.5);
    outColor = vec4(albedo * (ambient + diffuse * attenuation), 1.0);
}