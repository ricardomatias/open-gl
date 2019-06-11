#version 450

in vec3 Normal;
in vec3 FragPos;

out vec4 fragColor;

uniform vec3 lightPos;

void main() {
	fragColor = vec4(1.0);
}
