#version 450

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;
out vec4 fragColor;

struct Material {
	sampler2D diffuseTex1;
	sampler2D specularTex1;
	sampler2D reflectionTex1;
};

uniform Material material;

uniform samplerCube texCube;

uniform vec3 uViewPos;
uniform mat4 uView;

void main()
{
	float ratio = 1.0 / 2.42;

	vec3 nrm = normalize(normal);

	vec3 viewRay = normalize(fragPos - uViewPos);
	vec3 refractRay = refract(viewRay, normal, ratio);

	vec3 reflectColor = texture(texCube, refractRay).rgb;
	vec3 reflexTex = texture(material.reflectionTex1, texCoord).rgb;

	vec3 sumReflect = mix(reflectColor, reflexTex, 0.7);

	vec3 diffTex = texture(material.diffuseTex1, texCoord).rgb;
	vec3 specTex = texture(material.specularTex1, texCoord).rgb;

	vec3 modelTextureColor = 0.34 * diffTex + 0.75 * sumReflect * specTex;

	fragColor = vec4(reflectColor, 1.0);
}