#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

out vec3 FragPos;
out vec3 Normal;

void main() {
	gl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);
	FragPos = u_NormalMatrix * aPos;
	Normal = aNormal;
}
