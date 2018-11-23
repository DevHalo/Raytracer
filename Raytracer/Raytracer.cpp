#include "pch.h"
#include <iostream>
#include "FileReader.h"
#include "Ray.h"
#include "ppm.h"
#include <algorithm>
#include "include/glm/glm.hpp"

using namespace std;

unique_ptr<Parameters> params;
const int MAX_DEPTH = 3;

glm::vec3 raytrace(Ray ray)
{
	// If the ray has bounced more than 3 times, return black
	if (ray.Depth() > MAX_DEPTH) { return { 0,0,0 }; }

	// Formula for colour is:
	// Local Illumination (Ambient * Diffuse)
	// Sum of light contribution:
	// - If intersect, Use light + diffuse + specular
	// - No 
	// Reflections (From collision point recursively raytrace)
	glm::vec3 clocal, cshadow, crefl;
	
	bool inside = false;

	// CLAMP LIGHTING VALUES FROM LIGHTS UNIFORMLY
	// CLAMP FROM THE LARGEST VALUE [11, 1, 1] => [1, 1/11, 1/11]
	// OR ELSE LIGHTING WILL LOOK WEIRD

	std::unique_ptr<Quadratic> quad = ray.ShortestCollisionPoint(params->spheres);

	if (quad)
	{
		// If t1 is < -1, it is behind the near plane, thus we must use t2

		return quad->sphere->colour;

		return clocal + cshadow + crefl;
	}
		
	// No collision, return background colour
	return *params->back;
}

glm::vec3 shadowRays(glm::vec4 collisionPoint)
{
	glm::vec3 shadowColour = { 0, 0, 0 };

	for (auto it = params->lights.begin(); it != params->lights.end(); ++it)
	{
		// Create a ray from the collision point to the light
		glm::vec4 dir = (*it)->pos - collisionPoint;
		Ray sRay(collisionPoint, dir, 0);

		
	}

	return shadowColour;
}

// Entry point for raytracing application
int main(const int argc, char* arg[])
{
	// If the number of arguments is not 1
	if (argc != 2)
	{
		cout << "Invalid argument count" << endl << "Usage: raytracer.exe <filename> : filename must be .txt";
		return -1;
	}
	
	FileReader reader;
	params = reader.ReadFile(arg[1]);

	// If the file was not successfully read
	if (params == nullptr)
	{
		cout << "Failed to read file \"" << arg[1] << "\"";
		return -1;
	}
	else
	{
		// Store our array of pixels row, column
		std::vector<unsigned char> pixels = 
			std::vector<unsigned char>();

		float width = (*params->right - *params->left) / 2.0f;
		float height = (*params->top - *params->bottom) / 2.0f;

		int index = 0;
		for (int row = 0; row < *params->resY; row++)
		{
			for (int col = 0; col < *params->resX; col++)
			{
				// Get colour from primary ray and subsequent raytraces
				Ray pRay(params, width, height, col, row);
				glm::vec3 objectColour = raytrace(pRay);

				pixels.push_back(objectColour.r * 255.f);
				pixels.push_back(objectColour.g * 255.f);
				pixels.push_back(objectColour.b * 255.f);
				/*
				if ((params->spheres))
				{
					// Hit nothing, return background colour
					pixels.push_back(0.5f * 255);
					pixels.push_back(0.5f * 255);
					pixels.push_back(0.5f * 255);

					glm::vec3 pixelColour;
				}
				else
				{
					// Hit nothing, return background colour
					pixels.push_back(clamp((*params->back).r) * 255);
					pixels.push_back(clamp((*params->back).g) * 255);
					pixels.push_back(clamp((*params->back).b) * 255);
				}
				*/
			}
		}

		// Cast string to c-style string and save file to ppm
		const char* out = params->output.c_str();
		ppm::save_imageP6(*params->resX, *params->resY, out, &pixels[0]);

		return 0;
	}
}
