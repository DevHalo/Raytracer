#include "pch.h"
#include "Ray.h"
#include "FileReader.h"
#include "glm/glm.hpp"

glm::vec4 o = { 0.0f, 0.0f, 0.0f, 1.0f };	// Ray starting point, or eye
glm::vec4 d = { 0.0f, 0.0f, 0.0f, 1.0f };	// Direction of ray, intersects with near plane
float t = FLT_MAX;							// Scalar for direction

// r(t) = o + dt, t = {0, +inf}

int depth = 0;

// Casts a ray with supplied origin and direction
Ray::Ray(glm::vec4 origin, glm::vec4 direction)
{
	o.x = origin.x;
	o.y = origin.y;
	o.z = origin.z;

	d.x = direction.x;
	d.y = direction.y;
	d.z = direction.z;
}

// Casts a ray from the camera origin to the column and pixel location
Ray::Ray(std::unique_ptr<Parameters>& params, float width, float height, int col, int row)
{
	// Formula is eye + t(P(r,c) - eye) but eye is (0, 0, 0) therefore only P(r,c)
	// needs to be defined
	o.x = width * ((2.0f * col / *params->resX) - 1.0f);
	o.y = height * ((2.0f * row / *params->resY) - 1.0f);
	o.z = -*params->near;

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
			// Formula:
			// |d|^2t + 2(d . c)t + |c|^2 - 1 = 0 

			glm::vec4 c = o - (*it)->pos;

			// Get each component in at^2 + 2bt + c = 0
			float qA = dot(d, d);
			float qB = 2 * dot(d, c);
			float qC = dot(c, c) - 1;

			float discriminant = (qB * qB) - (4 * qA * qC);

			if (discriminant > 0)
			{
				
			}
			else if (discriminant == 0)
			{
				
			}
			else
			{
				
			}
		}
		else
		{
			// Ellipsoid, transform matrix to get inverse ray
		}
	}

	return closestSphere;
}