#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;

void main()
{
    fragPos = vec3(uModel * vec4(aPos, 1.0));
	texCoord = aTexCoords;
	normal = uNormalMatrix * aNormals;

    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0); 
} 