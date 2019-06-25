#version 450

in SHADER_BUS
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec4 fragPosLightSpace;
} frag_in;

out vec4 fragColor;

struct Material {
	sampler2D diffuseTex1;
    float     shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

uniform vec3 uViewPos;

uniform sampler2D shadowMap;

float calcShadow(vec4 fragPosLightSpace)
{
	// perform perspective divide - transform to clip-space
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // returns the fragment's light-space position in the range [-1,1]

	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0) return 0.0;

	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;

	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	  // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(frag_in.normal);
    vec3 lightDir = normalize(light.position - frag_in.fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // check whether current frag pos is in shadow
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 

	// PCF - Percentage-closer filtering
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }

    shadow /= 9.0; // number of times we sample
	
	return shadow;
}

void main() {
	vec3 result;
	vec2 uv = frag_in.texCoord;
	vec3 diffTex = texture(material.diffuseTex1, frag_in.texCoord).rgb;

	// ambient
	vec3 ambient = light.ambient * diffTex;

    // diffuse 
    vec3 norm = normalize(frag_in.normal);
	vec3 lightDir = normalize(light.position - frag_in.fragPos);

	float diffFactor = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = light.diffuse * diffFactor;

	// specular
	vec3 viewDir = normalize(uViewPos - frag_in.fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	
	float specFactor = pow(max(dot(viewDir, halfWayDir), 0.0), material.shininess);
    
	vec3 specular = light.specular * specFactor;

	// shadow
	float shadow = calcShadow(frag_in.fragPosLightSpace);

	result = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffTex;

//	result = (ambient + (diffuse * diffTex) + specular);

    fragColor = vec4(result, 1.0);
}