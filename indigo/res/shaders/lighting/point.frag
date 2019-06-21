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
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;

uniform vec3 u_viewPos;
uniform float u_time;

void main() {
	vec3 result;
	vec2 uv = texCoord;
	vec3 diffTex = texture(material.diffuseTex1, texCoord).rgb;

	// ambient
	vec3 ambient = light.ambient * diffTex;

    // diffuse 
    vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);

	float diffFactor = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = light.diffuse * diffFactor * diffTex;

	// specular
	vec3 viewDir = normalize(u_viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	vec3 specularMap = texture(material.specularTex1, texCoord).rgb;

	vec3 specular = light.specular * (specFactor * specularMap);  

	result += ambient + diffuse + specular;

	// attenuation
	float distance = length(light.position - fragPos);

	float attenuation = 1.0 / (
		light.constant +
		light.linear * distance + 
    	light.quadratic * (distance * distance)
	);  

    fragColor = vec4(result * attenuation, 1.0);
}

