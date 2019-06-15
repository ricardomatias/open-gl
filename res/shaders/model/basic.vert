#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main()
{
    texCoords = aTexCoords;    

    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}