#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNorm;

out gl_PerVertex {
	vec4 gl_Position;
};


void main() {
	gl_Position = vec4(position.xyz, 1.0);
	fragColor = color;
	fragNorm = normal;
}