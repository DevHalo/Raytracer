#include "pch.h"
#include "Ray.h"
#include "FileReader.h"
#include "glm/glm.hpp"

// Returns the depth of the current ray being traced
int Ray::Depth() const { return depth; }

// Returns the origin point of the ray
glm::vec4 Ray::Origin() const { return o; }

// Returns the non-normalized direction of the ray
glm::vec4 Ray::Direction() const { return d; }

// Clamps a colour uniformly
void Ray::ClampColour(glm::vec3& colour)
{
	if (colour.r <= 1.f && colour.g <= 1.f && colour.b <= 1.f) { return; }

	float maxColour = colour.r;

	// Determine largest colour value from r,g,b
	if (maxColour < colour.g) { maxColour = colour.g; }
	if (maxColour < colour.b) { maxColour = colour.b; }

	// Clamp by scaling each colour by the largest
	if (maxColour > 1.0f) { colour.r /= maxColour; colour.g /= maxColour; colour.b /= maxColour; }
}

// Casts a ray with supplied origin and direction
Ray::Ray(glm::vec4& origin, glm::vec4& direction, int _depth)
{
	// Starting at hit point t = 0.0001 to prevent numerical errors
	o = origin + direction * 0.0001f;
	o.w = 1.f;
	d = direction;
	d.w = 0.f;

	depth = _depth;
}

// Casts a ray from the camera origin to the column and pixel location
Ray::Ray(std::unique_ptr<Parameters>& params, float width, float height, int col, int row)
{
	o = glm::vec4(0.f, 0.f, 0.f, 1.f);

	// Formula is eye + t(P(r,c) - eye) but eye is (0, 0, 0) therefore only P(r,c)
	// needs to be defined
	d = glm::vec4(
		width * ((2.0f * col / *params->resX) - 1.0f),
		height * ((2.0f * row / *params->resY) - 1.0f), 
		-*params->near, 
		0.f
	);

	depth = 0;
}

// Returns a struct containing all components of the quadratic as well as
// the intercepts and whether or not to flip the normal (If the collision point is inside an object)
// Returns nullptr if no collision is found
std::unique_ptr<Quadratic> Ray::GetCollisionPoint(std::shared_ptr<Sphere>& sphere)
{
	glm::vec4 invO = o - sphere->pos;
	glm::vec4 invD = d;

	// If the sphere is non-canonical, we must transform the ray relative to the sphere
	if (sphere->scl.x != 1.f || sphere->scl.y != 1.f || sphere->scl.z != 1.f)
	{
		invO = sphere->inverseMatrix * invO;
		invD = sphere->inverseMatrix * invD;
	}

	// Formula:
	// |d|^2t + 2(d . c)t + |c|^2 - 1 = 0 

	// Drop w components and use cartesian space
	invO.w = 0.f;
	invD.w = 0.f;

	float A = dot(invD, invD);
	float B = dot(invD, invO);
	float C = dot(invO, invO) - 1.f;

	float discriminant = (B * B) - (A * C);
	Quadratic q;
	std::unique_ptr<Quadratic> qPtr;

	// Two collisions
	if (discriminant > 0)
	{
		discriminant = sqrt(discriminant);

		// We need to compare two values
		float t1 = (-B + discriminant) / A;
		float t2 = (-B - discriminant) / A;

		// If the sphere is behind the ray, ignore
		if (t1 < 0.f && t2 < 0.f) { return nullptr; }
		q = { A, B, C, t1, t2, sphere };

		return std::make_unique<Quadratic>(q);
	}

	// One collision
	if (discriminant == 0)
	{
		discriminant = sqrt(discriminant);

		// Doesn't matter which value we compute, compare with the shortest distance t
		float t1 = -B / A;

		// If the sphere is behind the ray, ignore
		if (t1 < 0.f) { return nullptr; }
		q = { A, B, C, t1, t1, sphere };

		return std::make_unique<Quadratic>(q);
	}

	// If no collision, return nullptr
	return qPtr;
}

// Returns a struct containing all the components of a quadratic of the
// closest intersection between a set of objects
// Returns nullptr if no collision is found
std::unique_ptr<Quadratic> Ray::ShortestCollisionPoint(std::vector<std::shared_ptr<Sphere>>& spheres)
{
	std::unique_ptr<Quadratic> quad;
	for (auto it = spheres.begin(); it != spheres.end(); ++it)
	{
		std::unique_ptr<Quadratic> tempQ = GetCollisionPoint(*it);

		// If there is an actual collision
		if (tempQ)
		{
			if (!quad)
			{
				// First collision that has occurred
				quad = std::move(tempQ);
			}
			else
			{
				// If the new collision is closer to the ray, we use this instead
				float mint1 = tempQ->t1 < tempQ->t2 ? tempQ->t1 : tempQ->t2;
				float mint2 = quad->t1 < quad->t2 ? quad->t1 : quad->t2;

				if (mint1 < mint2) { quad = std::move(tempQ); }
			}
		}
	}

	// If no collision, return nullptr
	return quad;
}

// Returns whether or not there is a collision between a ray and a sphere.
// Does not compute t values.
bool Ray::Collides(std::vector<std::shared_ptr<Sphere>>& spheres)
{
	for (auto it = spheres.begin(); it != spheres.end(); ++it)
	{
		std::unique_ptr<Quadratic> hit = GetCollisionPoint(*it);

		if (hit)
		{
			// If t1 or t2 is positive, that means the collision is in front of the ray
			if (hit->t1 >= 0.f || hit->t2 >= 0.f) { return true; }
		}
	}

	return false;
}
