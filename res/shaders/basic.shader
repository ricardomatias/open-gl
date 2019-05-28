#shader vertex
#version 410

layout (location = 0) in vec4 position;

void main() {
    gl_Position = position;
}

#shader fragment
#version 410

layout (location = 0) out vec4 outColor;

uniform vec4 u_Color;

void main() {
    outColor = u_Color;
}
