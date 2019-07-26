/*
 * * NormalsDemo
 * * InstancingDemo
 * * BasicComputeDemo
 */

#include "../demos/compute-basic/compute.h"

int main()
{
	BasicComputeDemo::Application app{ BasicComputeDemo::SCR_WIDTH, BasicComputeDemo::SCR_HEIGHT, "Compute Shaderz" };

	app.run();

	return 0;
}
