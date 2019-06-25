#version 450

in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D depthMap;

uniform float uNearPlane;
uniform float uFarPlane;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * uNearPlane * uFarPlane) / (uFarPlane + uNearPlane - z * (uFarPlane - uNearPlane));	
}

void main()
{
	float depthValue = texture(depthMap, texCoords).r;

	fragColor = vec4(vec3(LinearizeDepth(depthValue) / uFarPlane), 1.0); // perspective
    fragColor = vec4(vec3(depthValue), 1.0); // orthographic
}