#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 fragPos;
out vec3 normals;
out vec2 texCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;

void main()
{
    texCoords = aTexCoords;
	normals = uNormalMatrix * aNormal;
	fragPos = vec3(uModel * vec4(aPos, 1.0));

    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}