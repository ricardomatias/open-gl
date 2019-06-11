#version 450 core

//The control shader is responsible for calculating the values of the output control points and for setting
//the tessellation factors for the resulting patch that will be sent to the fixed-function tessellation engine.

layout (vertices = 3) out; // output control points

void main() {
	if (gl_InvocationID == 0) { // gl_InvocationID contains the index of the output patch vertex assigned to the shader invocation.
		// output tessellation factors
		gl_TessLevelInner[0] = 5.0;
		gl_TessLevelOuter[0] = 5.0;
		gl_TessLevelOuter[1] = 5.0;
		gl_TessLevelOuter[2] = 5.0;
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}