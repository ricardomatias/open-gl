#version 450

#define VERTICES 3

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
    float gl_ClipDistance[];
} gl_in[];


in SHADER_BUS {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
} geom_in[];

out SHADER_BUS
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
} geom_out;

uniform float uTime;

vec3 getNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	
	return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
	float mag = 2.0;
	vec3 dir = normal * max(sin(uTime) + 0.9, 0.0) * mag;

	return position + vec4(dir, 0.0);
}

void main()
{
	vec3 nrm = getNormal();

	for (int i = 0; i < VERTICES; i++) {
		gl_Position = explode(gl_in[i].gl_Position, nrm);
	
		geom_out.texCoord = geom_in[i].texCoord;
		geom_out.normal = geom_in[i].normal;
		geom_out.fragPos = geom_in[i].fragPos;
		
		EmitVertex();
	}

	EndPrimitive();
}