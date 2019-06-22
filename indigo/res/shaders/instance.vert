#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out SHADER_BUS {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
} vs_out;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;

void main()
{
    vs_out.texCoord = aTexCoords;
	vs_out.normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
	vs_out.fragPos = vec3(instanceMatrix * vec4(aPos, 1.0));

    gl_Position = uProj * uView * instanceMatrix * vec4(aPos, 1.0);
}