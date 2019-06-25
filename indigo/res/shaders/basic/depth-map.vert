#version 450

layout (location = 0) in vec3 aPos;

uniform mat4 uLightSpace;
uniform mat4 uModel;

void main()
{
	gl_Position = uLightSpace * uModel * vec4(aPos, 1.0);
}

