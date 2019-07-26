#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out SHADER_BUS {
	vec2 texCoord;
} vs_out;

void main() {
	vs_out.texCoord = aTexCoord;

	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}