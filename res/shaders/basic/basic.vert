#version 450 core

// 'offset' is an input vertex attribute
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
	vec4 position;
	vec2 texCoords;
} vs_out;

uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_Model;

void main() {
	// Add 'offset' to our hard-coded vertex position
	gl_Position = u_Proj * u_View * u_Model * aPosition;

	vs_out.position = aPosition;
	vs_out.texCoords = aTexCoords;
	// Output a fixed value for vs_color
}