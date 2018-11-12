#pragma once
#include <string>
#include "Sphere.h"
#include "Light.h"

struct Parameters
{
	float near, left, right, bottom, top;
	int resX, resY;
	int numSpheres, numLights;
	std::unique_ptr<Sphere> (*spheres)[];
	std::unique_ptr<Light> (*lights)[];
};

class FileReader
{
public:
	std::unique_ptr<Parameters> ReadFile(std::string filename);
};

