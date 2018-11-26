#pragma once
#include "include/glm/mat4x4.hpp"
#include "include/glm/vec3.hpp"
#include "include/glm/vec4.hpp"
#include <string>

// Container for a sphere object in the scene
struct Sphere
{
	std::string name;					// Name of the object
	glm::vec4 pos;						// Position of sphere in 3D space
	glm::vec4 scl;						// Scalar size of sphere
	glm::vec3 colour;					// RGB values of sphere
	float k_a, k_diff, k_spec, k_refl;	// Illumination model variables
	int n;								// Specular Component
	glm::mat4 transformMatrix;			// Transformation matrix for this sphere
	glm::mat4 inverseMatrix;			// Stores the transformed inverse matrix for this sphere
};