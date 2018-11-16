#include "pch.h"
#include <iostream>
#include "FileReader.h"
#include "Ray.h"

using namespace std;

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
		std::vector<std::vector<glm::vec3>> pixelColour(
			*params->resY, std::vector<glm::vec3>(*params->resX));

		for (int row = 0; row < *params->resY; row++)
		{
			for (int col = 0; col < *params->resX; col++)
			{
				Ray ray;
			}
		}

		return 0;
	}
}