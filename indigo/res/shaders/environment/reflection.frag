#version 450

in vec3 fragPos;
in vec3 normal;
out vec4 fragColor;

uniform sampler2D tex0;
uniform samplerCube texCube;

uniform vec3 uViewPos;
uniform mat4 uView;

void main()
{
	vec3 viewRay = normalize(fragPos - uViewPos);
	vec3 reflexRay = reflect(viewRay, normalize(normal));

	vec3 tex = texture(texCube, reflexRay).rgb;

	fragColor = vec4(tex, 1.0);
}