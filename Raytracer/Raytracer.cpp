#include "pch.h"
#include <iostream>
#include "FileReader.h"
#include "Ray.h"
#include "ppm.h"
#include <algorithm>

using namespace std;

// Returns a value clamped between 0.0f and 1.0f
float clamp(float val)
{
	return std::max(0.0f, std::min(1.0f, val));
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
	unique_ptr<Parameters> params = reader.ReadFile(arg[1]);

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
				Ray pRay(params, width, height, col, row);
				if (pRay.SphereIntersect(params->spheres))
				{


					glm::vec3 pixelColour;
					cout << "Collision at: " << col << ", " << row;
				}
				else
				{
					// Hit nothing, return background colour
					pixels.push_back(clamp((*params->back).r) * 255);
					pixels.push_back(clamp((*params->back).g) * 255);
					pixels.push_back(clamp((*params->back).b) * 255);
				}
			}
		}

		// Cast string to c-style string and save file to ppm
		const char* out = params->output.c_str();
		ppm::save_imageP3(*params->resX, *params->resY, out, &pixels[0]);

		return 0;
	}
}
