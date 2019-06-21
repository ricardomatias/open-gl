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
	vec3 spotlight;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float innerCutoff;
	float outerCutoff;
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

	// attenuation / intensity

	// Because the values are in the cosine domain.
	// Larger angles have smaller coefficients than smaller angles.
	// f.ex. cos(90) < cos(12.5) which isn't what we want (GOOD FOR SHADOWS THOUGHT :D)
	vec3 spotlightDir = normalize(-light.spotlight);

	float cosTheta = dot(light.spotlight, -lightDir); // both vectors pointing TO the light source NOT FROM
	float cosEpsilon   = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((cosTheta - light.outerCutoff) / cosEpsilon, 0.0, 1.0); 
	
	diffuse *= intensity;
	specular *= intensity;

	result += ambient + diffuse + specular;

    fragColor = vec4(result, 1.0);
}

