#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 inPosition;
layout(location = 2) in float inRadius;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1);
    float distance = inRadius - length(inPosition);
    float circle = smoothstep(0.0, 1, distance);
    circle *= smoothstep(inRadius + 1, inRadius, distance);

	if (circle == 0.0)
		discard;

    outColor.a *= circle;
}