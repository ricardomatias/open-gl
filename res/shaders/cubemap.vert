#version 450

layout(location = 0) in vec3 aPos;

out vec3 textureDir;

uniform mat4 uProj;
uniform mat4 uView;

void main()
{
	textureDir = aPos;

	vec4 pos = uProj * uView * vec4(aPos, 1.0);

    gl_Position = pos.xyww;
}