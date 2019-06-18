#version 450

out vec4 fragColor;

in vec3 fragPos;
in vec3 normals;
in vec2 texCoords;

struct Material {
    sampler2D diffuseTexture1;
    sampler2D specularTexture1;
    float     shininess;
};

struct DirectionalLight {
	vec3 direction;

	vec3 color;
};

struct PointLight {
	vec3 position;

	float ambient;
	float diffuse;
	float specular;

	vec3 color;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;

	float ambient;
	float diffuse;
	float specular;

	vec3 color;

	float innerCutoff;
	float outerCutoff;
};

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform vec3 camPos;

float attenuatePointLight(vec3 lightDir)
{
	float d = length(lightDir);

	float kC = pointLight.constant;
	float kL = pointLight.linear * d;
	float kQ = pointLight.quadratic * d * d;

	return 1.0f / (kC + kL + kQ);
}

void main()
{    
	vec3 result;
	vec3 norm = normalize(normals);
	vec3 lightRay = normalize(fragPos - spotLight.position);

	// DIFFUSE
	float diffuseFactor = 1.;

	float diffuseIntensity = max(dot(-lightRay, norm), 0.0);
	vec3 diffTex = texture(material.diffuseTexture1, texCoords).rgb;

	vec3 diffuse = spotLight.diffuse * diffuseFactor * diffuseIntensity * diffTex;

	// SPECULAR
	vec3 camRay = normalize(fragPos - camPos);
	vec3 reflexRay = reflect(-lightRay, norm);
	float specIntensity = max(dot(reflexRay, camRay), 0.0);

	float specFactor = pow(specIntensity, material.shininess);
	
	vec3 specTex = texture(material.specularTexture1, texCoords).rgb;

	vec3 specular = spotLight.specular * specFactor * specTex;

	// AMBIENT
	vec3 ambient = spotLight.ambient * diffTex;

	// ATTENUATION
	vec3 spotLightRay = normalize(spotLight.direction);
	
	float cosTheta = max(dot(lightRay, spotLightRay), 0.0);
	float cosEpsilon = spotLight.innerCutoff - spotLight.outerCutoff; // reversed coz cosine
	float attenuation = clamp((cosTheta - spotLight.outerCutoff) / cosEpsilon, 0., 1.);

	diffuse *= attenuation;
	specular *= attenuation;

	result = (ambient + diffuse + specular) * spotLight.color;

    fragColor = vec4(result, 1.0);
}