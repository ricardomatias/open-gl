#version 450

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D tex0;

void main()
{
	fragColor = texture(tex0, texCoords);
}
