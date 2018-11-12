#pragma once
#include "Sphere.h"
#include "include/glm/vec3.hpp"
#include "string"
#include "Colour.h"

// Container for a sphere object in the scene
struct Sphere
{
	std::string name;					// Name of the object
	glm::vec3 pos;						// Position of sphere in 3D space
	glm::vec3 scl;						// Scalar size of sphere
	Colour colour;						// RGB values of sphere
	float k_a, k_diff, k_spec, k_refl;	// Illumination model variables
	int n;								// Specular Component

	// Creates a sphere with the specified parameters
	Sphere(std::string _name, glm::vec3 _pos, glm::vec3 _scl, Colour _colour, float _k_a, float _k_diff, float _k_spec, float _k_refl) :
		name(_name), pos(_pos), scl(_scl), colour(_colour), k_a(_k_a), k_diff(_k_diff), k_spec(_k_spec), k_refl(_k_refl) {};
};