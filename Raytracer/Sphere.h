#pragma once
#include "include/glm/vec3.hpp"
#include <string>

// Container for a sphere object in the scene
struct Sphere
{
	std::string name;					// Name of the object
	glm::vec3 pos;						// Position of sphere in 3D space
	glm::vec3 scl;						// Scalar size of sphere
	glm::vec3 colour;					// RGB values of sphere
	float k_a, k_diff, k_spec, k_refl;	// Illumination model variables
	int n;								// Specular Component
};