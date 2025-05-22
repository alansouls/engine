#version 450

struct InstanceData {
    mat4 model;
    vec3 inColor;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) readonly buffer StorageBufferObject {
    InstanceData instanceData[];
} ssbo;

layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * ssbo.instanceData[gl_InstanceIndex].model * vec4(inPosition, 0.0, 1.0); //  vec4(inPosition, 0.0, 1.0);
    fragColor = ssbo.instanceData[gl_InstanceIndex].inColor;
}