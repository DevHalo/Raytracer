#pragma once
#include "Sphere.h"
#include "Light.h"
#include <vector>

struct Parameters
{
	std::shared_ptr<float> near, left, right, bottom, top;	// The near plane and frustrum
	std::shared_ptr<int> resX, resY;						// X columns and Y rows
	std::vector<std::shared_ptr<Sphere>> spheres;			// Dynamic array storing spheres
	std::vector<std::shared_ptr<Light>> lights;				// Dynamic array storing lights
	std::shared_ptr<glm::vec3> back;						// Background colour
	std::shared_ptr<glm::vec3> ambient;						// Ambient colour
	std::string output;										// Output file name
};

// Handles File IO
class FileReader
{
public:
	std::unique_ptr<Parameters> ReadFile(std::string filename);
private:
	std::shared_ptr<float> ConvertToFloat(std::string strVal) const;
};
