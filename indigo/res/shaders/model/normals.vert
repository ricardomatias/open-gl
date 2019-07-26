#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

out SHADER_BUS {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec3 tangentLightDir;
	vec3 tangentViewDir;
	vec3 tangentFragPos;
} vs_out;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;

uniform vec3 uViewPos;
uniform vec3 uLightPos;

void main()
{
    vs_out.texCoord = aTexCoords;
	vs_out.normal = uNormalMatrix * aNormal;
	vs_out.fragPos = vec3(uModel * vec4(aPos, 1.0));
	
    vec3 N = normalize(uNormalMatrix * aNormal);
	vec3 T = normalize(uNormalMatrix * aTangent);
	vec3 B = normalize(uNormalMatrix * aBiTangent);

	// then retrieve perpendicular vector B with the cross product of T and N
//	vec3 B = cross(N, T);

	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);

	// TBN must form a right handed coord system.
    // Some models have symetric UVs. Check and fix.
	if (dot(cross(N, T), B) < 0.0) T = T * -1.0;
    
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.tangentLightDir = (uLightPos - vs_out.fragPos) * TBN;
    vs_out.tangentViewDir  = (uViewPos - vs_out.fragPos) * TBN;
    vs_out.tangentFragPos  = TBN * vs_out.fragPos;

    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}