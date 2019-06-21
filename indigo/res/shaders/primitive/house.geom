#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in gl_PerVertex
{
    vec4  gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
} gl_in[];

in VS_OUT {
    vec3 color;
} geom_in[];

out vec3 fColor; 

const float off = 0.4;

const float halfOff = off / 2.;

void main() {    
	fColor = geom_in[0].color;

	gl_Position = gl_in[0].gl_Position + vec4(-halfOff, -halfOff, 0.0, 0.0); // bottom left
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( halfOff, -halfOff, 0.0, 0.0); // bottom right
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( -halfOff, halfOff, 0.0, 0.0); // upper left
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( halfOff, halfOff, 0.0, 0.0); // upper right
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.0, off, 0.0, 0.0); // top middle
    EmitVertex();
    
    EndPrimitive();
} 