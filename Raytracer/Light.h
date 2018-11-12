#pragma once
#include <string>
#include "include/glm/vec3.hpp"
#include "Colour.h"

// Container for a light element in the scene
struct Light
{
	std::string name;		// Name of the object
	glm::vec3 pos;			// Position of the object in 3D space
	Colour intensity;		// RGB values of the light's intensity

	// Creates a light with the specified parameters
	Light(std::string _name, glm::vec3 _pos, Colour _intensity) :
		name(_name), pos(_pos), intensity(_intensity) {};
};