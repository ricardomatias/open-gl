#version 450

layout (triangles) in;
layout (line_strip, max_vertices = 6)  out;

in gl_PerVertex
{
	vec4 gl_Position;
} gl_in[];

in SHADER_BUS
{
	vec3 normal;
} geom_in[];

const float MAG = 0.1;

void main()
{
	for (int i = 0; i < 3; i++) {
		vec3 nrm = geom_in[i].normal;

		gl_Position = gl_in[i].gl_Position;
		EmitVertex();

		gl_Position = gl_in[i].gl_Position + vec4(nrm, 0.0) * MAG;
		EmitVertex();

		EndPrimitive();
	}
}

