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

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {    
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;  
};  


uniform Material material;

// LIGHTS
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];


uniform vec3 u_viewPos;
uniform float u_time;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  

void main() {
	vec3 result;
	// properties
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(u_viewPos - fragPos);

    // phase 1: Directional lighting
    //vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
	}
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    fragColor = vec4(result, 1.0);
}


vec3 CalcLightOnTexture(Material material, vec3 ambient, vec3 diffuse, vec3 specular, float diffFactor, float specFactor)
{
    ambient  *= vec3(texture(material.diffuse, texCoord));
    diffuse  *= diffFactor * vec3(texture(material.diffuse, texCoord));
    specular *= specFactor * vec3(texture(material.specular, texCoord));

    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	// combine results
	return CalcLightOnTexture(material, light.ambient, light.diffuse, light.specular, diff, spec);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (
		light.constant +
		light.linear * distance + 
  		light.quadratic * (distance * distance)
	);    

    // combine results
	return CalcLightOnTexture(material, light.ambient, light.diffuse, light.specular, diff, spec) * attenuation;
} 