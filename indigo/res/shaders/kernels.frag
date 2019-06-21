#version 450

#define KERNEL_SIZE 9

in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D tex0;

const float offset = 1.0 / 300.0;

const float sharpenKernel[9] = float[](
	-1, -1, -1,
	-1,  9, -1,
	-1, -1, -1
);

const float blurKernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

const float edgeKernel[9] = float[](
	1,  1, 1,
	1, -8, 1,
	1,  1, 1
);

void main()
{
	vec2 offsets[KERNEL_SIZE] = vec2[KERNEL_SIZE](
		vec2(-offset, offset), // top-left
		vec2(0.0, offset), // top-center
		vec2(offset, offset), // top-right
		vec2(-offset, 0.0), // middle-left
		vec2(0.0, 0.0), // middle-center
		vec2(offset, 0.0), // middle-right
		vec2(-offset, -offset), // bottom-left
		vec2(0.0, -offset), // bottom-center
		vec2(offset, -offset) // bottom-right
	);

	vec3 samples[KERNEL_SIZE];

	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		samples[i] = texture(tex0, texCoords.st + offsets[i].st).rgb;
	}

	vec3 color = vec3(0.0);

	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		color += samples[i] * edgeKernel[i];
	}

	fragColor = vec4(color, 1.0);
}