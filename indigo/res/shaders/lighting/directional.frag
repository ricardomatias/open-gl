#version 450

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

struct Material {
	sampler2D diffuseTex1;
	sampler2D specularTex1;
	sampler2D reflectionTex1;
    float     shininess;
};

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

uniform vec3 uViewPos;

void main() {
	vec3 result;
	vec2 uv = texCoord;
	vec3 diffTex = texture(material.diffuseTex1, texCoord).rgb;

	// ambient
	vec3 ambient = light.ambient * diffTex;

    // diffuse 
    vec3 norm = normalize(normal);
	vec3 lightDir = normalize(-light.direction);

	float diffFactor = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = light.diffuse * diffFactor * diffTex;

	// specular
	vec3 viewDir = normalize(uViewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	vec3 specularMap = texture(material.specularTex1, texCoord).rgb;

	vec3 specular = light.specular * (specFactor * specularMap);  

	result += ambient + diffuse + specular;

    fragColor = vec4(result, 1.0);
}

