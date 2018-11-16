#pragma once
#include <string>
#include "include/glm/vec3.hpp"

// Container for a light element in the scene
struct Light
{
	std::string name;		// Name of the object
	glm::vec3 pos;			// Position of the object in 3D space
	glm::vec3 intensity;	// RGB values of the light's intensity
};