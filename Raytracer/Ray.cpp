#include "pch.h"
#include "Ray.h"
#include "FileReader.h"
#include <iostream>
#include "glm/glm.hpp"

glm::vec4 s = { 0.0f, 0.0f, 0.0f, 1.0f };	// Ray starting point, or eye
glm::vec4 c = { 0.0f, 0.0f, 0.0f, 1.0f };	// Direction of ray, intersects with near plane
float t = FLT_MAX;							// Scalar for direction

// r(t) = a + ct, t = {0, +inf}

int depth = 0;

// Casts a ray with supplied origin and direction
Ray::Ray(glm::vec4 origin, glm::vec4 direction)
{
	s.x = origin.x;
	s.y = origin.y;
	s.z = origin.z;

	c.x = direction.x;
	c.y = direction.y;
	c.z = direction.z;
}

// Casts a ray from the camera origin to the column and pixel location
Ray::Ray(std::unique_ptr<Parameters>& params, float width, float height, int col, int row)
{
	// Formula is eye + t(P(r,c) - eye) but eye is (0, 0, 0) therefore only P(r,c)
	// needs to be defined
	c.x = width * ((2.0f * col / *params->resX) - 1.0f);
	c.y = height * ((2.0f * row / *params->resY) - 1.0f);
	c.z = -*params->near;

	depth = 0;
}

// Attempts to find a collision point with
std::shared_ptr<Sphere> Ray::SphereIntersect(std::vector<std::shared_ptr<Sphere>>& spheres) const
{
	float tempT;
	std::shared_ptr<Sphere> closestSphere = nullptr;
	for (std::vector<std::shared_ptr<Sphere>>::iterator it = spheres.begin(); it != spheres.end(); ++it)
	{
		// If the sphere is canonical, we do not need to do any transformations
		if ((*it)->scl.x == 1 && (*it)->scl.y == 1 && (*it)->scl.z == 1)
		{
			// Get each component in at^2 + 2bt + c = 0
			//glm::vec4 S = a - (*it)->pos;	// Origin of eye minus origin of circle
			float a = dot(c, c);	// 
			float b = 2 * dot(c, s);
			float c = dot(s, s) - 1;

			// Find Discriminant
			float discriminant = b * b - a * c;
			if (discriminant == 0)
			{
				tempT = -b / a;
				closestSphere = *it;

				std::cout << "COLLISION: " << (*it)->name << " AT: " << tempT;
			}
			else if (discriminant > 0)
			{
				tempT = (-b + discriminant) / a;
				float tempT2 = (-b - discriminant) / a;
				closestSphere = *it;
				
				std::cout << "COLLISION: " << (*it)->name << " AT: " << tempT << ", " << tempT2;
				
			}
		}
		else
		{
			// Ellipsoid, transform matrix to get inverse ray
		}
	}

	return closestSphere;
}