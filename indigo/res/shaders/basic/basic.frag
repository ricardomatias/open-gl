#version 450 core

in VS_OUT {
	vec4 position;
	vec2 texCoords;
} fg_in;

// Output to the framebuffer
out vec4 color;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main(void) {
// Simply assign the color we were given by the vertex shader to our output
	color = mix(texture(tex0, fg_in.texCoords), texture(tex1, fg_in.texCoords), 0.5);
}