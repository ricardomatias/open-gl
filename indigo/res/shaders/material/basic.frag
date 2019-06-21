#version 450

in vec3 FragPos;
in vec3 Normal;

out vec4 fragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;  

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;


void main() {
	// ambient
    vec3 ambient = light.ambient * material.ambient;
    
	ambient *= lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffFactor = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = light.diffuse * (diffFactor * material.diffuse);

	diffuse *= lightColor;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	vec3 specular = light.specular * (spec * material.specular);  

	specular *= lightColor;
        
    vec3 result = ambient + diffuse + specular;

    fragColor = vec4(result, 1.0);
}