#version 450

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
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

	// ambient
	vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

    // diffuse 
    vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);

	float diffFactor = max(dot(norm, lightDir), 0.0);

	vec3 diffuseMap = texture(material.diffuse, texCoord).rgb;

	vec3 diffuse = light.diffuse * diffFactor * diffuseMap;

	// specular
	vec3 viewDir = normalize(u_viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	vec3 specularMap = texture(material.specular, texCoord).rgb;

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

