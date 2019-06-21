#version 450

in vec3 textureDir; // direction vector representing a 3D texture coordinate
out vec4 fragColor;

uniform samplerCube texCube;

void main()
{
	fragColor = texture(texCube, textureDir);
}