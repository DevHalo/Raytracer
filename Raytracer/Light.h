#pragma once
#include <string>
#include "include/glm/vec4.hpp"

// Container for a light element in the scene
struct Light
{
	std::string name;		// Name of the object
	glm::vec4 pos;			// Position of the object in 3D space
	glm::vec4 intensity;	// RGB values of the light's intensity
};