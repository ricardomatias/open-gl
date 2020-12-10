#version 430

layout (location = 0) in vec3 aPos;

out vec3 Position;

uniform mat4 uModelView;
uniform mat4 uModelViewProj;
uniform mat3 uNormalMatrix;

void main() {
	Position = (uModelView * vec4(aPos, 1.0)).xyz;

	gl_Position = uModelViewProj * vec4(aPos, 1.0);
}