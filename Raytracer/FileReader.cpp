#include "pch.h"
#include "FileReader.h"
#include <iostream>
#include <fstream>
#include "Sphere.h"
#include "Light.h"

// Takes in the name of the file and returns a pointer carrying all the parameters
std::unique_ptr<Parameters> FileReader::ReadFile(std::string filename)
{
	std::ifstream file(filename);
	std::unique_ptr<Parameters> param;

	if (file.is_open())
	{
		file.close();
		return param;
	}
	else
	{
		return NULL;
	}
}