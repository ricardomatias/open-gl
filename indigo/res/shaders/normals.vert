#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out SHADER_BUS {
	vec3 normal;
} vs_out;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;

void main()
{
	vs_out.normal = normalize(vec3(uProj * vec4(uNormalMatrix * aNormal, 0.0)));

    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}