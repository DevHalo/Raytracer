#pragma once
#include <string>
#include "Sphere.h"
#include "Light.h"
#include "Colour.h"

struct Parameters
{
	std::unique_ptr<float> near, left, right, bottom, top;
	std::unique_ptr<int> resX, resY, numSpheres, numLights;
	std::unique_ptr<Sphere>(*spheres)[];
	std::unique_ptr<Light>(*lights)[];
	std::unique_ptr<Colour> back = nullptr;
	std::unique_ptr<Colour> ambient = nullptr;
	std::string output;
};

class FileReader
{
public:
	std::unique_ptr<Parameters> ReadFile(std::string filename);
private:
	bool IsCommand(std::string buffer);
};

