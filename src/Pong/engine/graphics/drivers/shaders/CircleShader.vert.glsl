#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in float inRadius;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 outPosition;
layout(location = 2) out float outRadius;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
    outPosition = (inPosition).xy;
    outRadius = inRadius;
}