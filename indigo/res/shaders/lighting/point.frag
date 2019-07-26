#version 450

in SHADER_BUS
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec3 tangentLightDir;
	vec3 tangentViewDir;
	vec3 tangentViewPos;
} frag_in;

out vec4 fragColor;

struct Material {
	sampler2D diffuseMap1;
	sampler2D specularMap1;
	sampler2D reflectionMap1;

	sampler2D normalMap1;
    float     shininess;
};

struct Light {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;

uniform float uTime;

void main() {
	vec3 result;
	vec3 diffTex = texture(material.diffuseMap1, frag_in.texCoord).rgb;

	// ambient
	vec3 ambient = light.ambient * diffTex;

    // diffuse
	vec3 normalTex = texture(material.normalMap1, frag_in.texCoord).rgb;
    vec3 norm = normalize(normalTex * 2.0 - 1.0);
//	vec3 norm = normalize(frag_in.normal);
	vec3 lightDir = normalize(frag_in.tangentLightDir);

	float diffFactor = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = light.diffuse * diffFactor * diffTex;

	// specular
	vec3 viewDir = normalize(frag_in.tangentViewDir);
	vec3 halfVec = normalize(lightDir + viewDir);

	float specFactor = pow(max(dot(viewDir, halfVec), 0.0), material.shininess);
    
	vec3 specularMap = texture(material.specularMap1, frag_in.texCoord).rgb;

	vec3 specular = light.specular * (specFactor * specularMap);  

	result += ambient + diffuse;

	// attenuation
	float distance = length(frag_in.tangentViewDir);

	float attenuation = 1.0 / (
		light.constant +
		light.linear * distance + 
    	light.quadratic * (distance * distance)
	);  

    fragColor = vec4(norm, 1.0);
}

