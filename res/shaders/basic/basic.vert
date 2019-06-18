#version 450 core

// 'offset' is an input vertex attribute
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;

void main() {
	// Add 'offset' to our hard-coded vertex position
	gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);

	texCoords = aTexCoords;
	// Output a fixed value for vs_color
}