#version 450

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D diffuseTexture1;

void main()
{    
    fragColor = texture(diffuseTexture1, texCoords);
}