#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

void main() {
	fragPos = vec3(u_Model * vec4(aPos, 1.0));
	texCoord = aTexCoord;
	normal = u_NormalMatrix * aNormal;

	gl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);
}
