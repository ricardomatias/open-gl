#shader vertex
#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

out vec2 v_TexCoords;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * position;
    v_TexCoords = texCoord; 
}

#shader fragment
#version 410

layout (location = 0) out vec4 outColor;

in vec2 v_TexCoords;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main() {
    vec4 texColor = texture(u_Texture, v_TexCoords);
    outColor = texColor;
}
