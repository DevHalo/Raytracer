#include "pch.h"
#include "Ray.h"
#include "FileReader.h"
#include "glm/glm.hpp"

glm::vec4 o = { 0.0f, 0.0f, 0.0f, 1.0f };	// Ray starting point, or eye
glm::vec4 d = { 0.0f, 0.0f, 0.0f, 0.0f };	// Direction of ray, intersects with near plane
float t = FLT_MAX;							// Scalar for direction

// r(t) = o + dt, t = {0, +inf}

int depth = 0;

// Returns the depth of the current ray being traced
int Ray::Depth() { return depth; }

// Returns the origin point of the ray
glm::vec4 Ray::Origin() const { return o; }

// Returns the non-normalized direction of the ray
glm::vec4 Ray::Direction() const { return d; }

// Sets a new direction vector for the ray
void Ray::SetDirection(glm::vec4 newDirection) { d = newDirection; }

// Clamps a colour uniformly
void Ray::ClampColour(glm::vec3 colour)
{
	float maxColour = colour.r;

	// Determine largest colour value from r,g,b
	if (maxColour < colour.g) { maxColour = colour.g; }
	if (maxColour < colour.b) { maxColour = colour.b; }

	// Clamp by scaling each colour by the largest
	if (maxColour > 1.0f) { colour.r /= maxColour; colour.g /= maxColour; colour.b /= maxColour; }
}

// Casts a ray with supplied origin and direction
Ray::Ray(glm::vec4 origin, glm::vec4 direction, int _depth)
{
	o.x = origin.x;
	o.y = origin.y;
	o.z = origin.z;

	d.x = direction.x;
	d.y = direction.y;
	d.z = direction.z;

	depth = _depth;
}

// Casts a ray from the camera origin to the column and pixel location
Ray::Ray(std::unique_ptr<Parameters>& params, float width, float height, int col, int row)
{
	// Formula is eye + t(P(r,c) - eye) but eye is (0, 0, 0) therefore only P(r,c)
	// needs to be defined
	d.x = width * ((2.0f * col / *params->resX) - 1.0f);
	d.y = height * ((2.0f * row / *params->resY) - 1.0f);
	d.z = -*params->near;

	depth = 0;
}

// Returns a struct containing all components of the quadratic as well as
// the intercepts and whether or not to flip the normal (If the collision point is inside an object)
// Returns nullptr if no collision is found
std::unique_ptr<Quadratic> Ray::GetCollisionPoint(std::shared_ptr<Sphere> sphere)
{
	glm::vec4 invDir = d;
	
	// If the sphere is non-canonical, we must transform the ray relative to the sphere
	if (sphere->scl.x != 1 || sphere->scl.y != 1 || sphere->scl.z != 1) { invDir /= (sphere->scl * sphere->scl); }

	// Formula:
	// |d|^2t + 2(d . c)t + |c|^2 - 1 = 0 

	glm::vec4 c = o - sphere->pos;

	// Get each component in At^2 + 2Bt + C = 0
	//float A = dot(invDir, invDir);
	//float B = 2 * dot(invDir, c);
	//float C = dot(c, c) - 1;

	float A =
		((d.x * d.x) / (sphere->scl.x * sphere->scl.x)) +
		((d.y * d.y) / (sphere->scl.y * sphere->scl.y)) +
		((d.z * d.z) / (sphere->scl.z * sphere->scl.z));

	float B =
		((2.f * d.x * sphere->pos.x) / (sphere->scl.x * sphere->scl.x)) +
		((2.f * d.y * sphere->pos.y) / (sphere->scl.y * sphere->scl.y)) +
		((2.f * d.z * sphere->pos.z) / (sphere->scl.z * sphere->scl.z));

	float C =
		((sphere->pos.x * sphere->pos.x) / (sphere->scl.x * sphere->scl.x)) +
		((sphere->pos.y * sphere->pos.y) / (sphere->scl.y * sphere->scl.y)) +
		((sphere->pos.z * sphere->pos.z) / (sphere->scl.z * sphere->scl.z));

	float discriminant = (B * B) - (4 * A * C);
	Quadratic q;
	std::unique_ptr<Quadratic> qPtr;

	if (discriminant > 0)
	{
		discriminant = sqrt(discriminant);

		// We need to compare two values
		float t1 = (-B + discriminant) / A;
		float t2 = (-B - discriminant) / A;

		q = { A, B, C, t1, t2, sphere };
		qPtr = std::make_unique<Quadratic>(q);
	}

	if (discriminant == 0)
	{
		discriminant = sqrt(discriminant);

		// Doesn't matter which value we compute, compare with the shortest distance t
		float t1 = (-B + discriminant) / A;

		q = { A, B, C, t1, t1, sphere };
		qPtr = std::make_unique<Quadratic>(q);
	}

	// No collision, return nullptr
	return qPtr;
}

// Returns a struct containing all the components of a quadratic of the
// closest intersection between a set of objects
// Returns nullptr if no collision is found
std::unique_ptr<Quadratic> Ray::ShortestCollisionPoint(std::vector<std::shared_ptr<Sphere>> spheres)
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
				// If the new collision is closer to the ray, we reuse this instead
				if (tempQ < quad)
				{
					quad = std::move(tempQ);
				}
			}
		}
	}

	return quad;
}