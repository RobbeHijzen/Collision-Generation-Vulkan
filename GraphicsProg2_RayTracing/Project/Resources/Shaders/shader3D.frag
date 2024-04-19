#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in mat4 fragWorldMatrix;

layout(location = 0) out vec4 outColor;

vec3 lightDirection = vec3(-0.577, -0.577, -0.557);

void main() 
{
    vec3 normal = normalize(fragNormal);
    normal = mat3(fragWorldMatrix) * normal;
    
    float cosineLaw = dot(normal, -lightDirection);
    
    outColor = cosineLaw * texture(texSampler, fragTexCoord);
}