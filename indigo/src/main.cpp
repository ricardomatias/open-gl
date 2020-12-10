/*
 * * NormalsDemo
 * * InstancingDemo
 * * BasicComputeDemo
 */

#include "../demos/compute-particles-simple/compute-particles-simple.h"

int main()
{
	ComputeParticlesSimple::Application app{ ComputeParticlesSimple::SCR_WIDTH, ComputeParticlesSimple::SCR_HEIGHT, "Compute Particles (Simple)" };

	app.run();

	return 0;
}
