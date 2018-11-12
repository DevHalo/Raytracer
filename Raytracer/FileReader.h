#pragma once
#include <string>
#include "Sphere.h"
#include "Light.h"

struct Parameters
{
	float near, left, right, bottom, top;
	int resX, resY;
	int numSpheres, numLights;

	Parameters(float _near, float _left, float _right, float _bottom, float _top, int _resX, int _resY, int _numSpheres, int _numLights, Sphere _spheres[], Light _lights[]) :
		near(near), left(_left), right(_right), bottom(_bottom), top(_top), resX(_resX), resY(_resY), numSpheres(_numSpheres) {};
};

class FileReader
{
public:
	Parameters * ReadFile(std::string filename);
};

