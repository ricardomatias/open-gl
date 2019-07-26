#version 450

in SHADER_BUS {
	vec2 texCoord;
} frag_in;

out vec4 fragColor;

uniform sampler2D tex0;

void main() {
	fragColor = texture(tex0, frag_in.texCoord);
}