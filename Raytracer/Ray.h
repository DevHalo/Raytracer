#pragma once
#include "FileReader.h"

// Data structure to store the quadratic equation formed from a collision
struct Quadratic
{
	float A;						// A component of the quadratic formula
	float B;						// B component of the quadratic formula
	float C;						// C component of the quadratic formula
	float t;						// The intercept
	std::shared_ptr<Sphere> sphere;	// The sphere involved with the collision
	bool flip;						// The normal should be flipped if collision is inside the sphere
};

// Ray class which casts a ray from an origin point towards a direction multiplied by a scalar
class Ray
{
public:
	int Depth() const;
	static void ClampColour(glm::vec3& colour);
	Ray(glm::vec4 origin, glm::vec4 direction, int _depth);
	Ray(std::unique_ptr<Parameters>& params, float width, float height, int col, int row);
	std::unique_ptr<Quadratic> ShortestCollisionPoint(std::vector<std::shared_ptr<Sphere>> spheres);
	bool Collides(std::vector<std::shared_ptr<Sphere>> spheres);
	glm::vec4 Origin() const;
	glm::vec4 Direction() const;
private:
	std::unique_ptr<Quadratic> GetCollisionPoint(std::shared_ptr<Sphere> sphere);
	glm::vec4 o = { 0.f, 0.f, 0.f, 1.f };	// Ray starting point, or eye
	glm::vec4 d = { 0.f, 0.f, 0.f, 0.f };	// Direction of ray, intersects with near plane
	float t = FLT_MAX;						// Scalar for direction
	int depth = 0;							// The recursive depth of the ray
};
