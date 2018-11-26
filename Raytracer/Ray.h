#pragma once
#include "FileReader.h"

// Data structure to store the quadratic equation formed from a collision
struct Quadratic
{
	float A;
	float B;
	float C;
	float t1;
	float t2;
	std::shared_ptr<Sphere> sphere;

	bool operator<(Quadratic other) const
	{
		float minT1 = t1 < t2 ? t1 : t2;
		float minT2 = other.t1 < other.t2 ? t1 : t2;

		return minT1 < minT2;
	}
};

class Ray
{
public:
	int Depth();
	static void ClampColour(glm::vec3 colour);
	Ray(glm::vec4 origin, glm::vec4 direction, int _depth);
	Ray(std::unique_ptr<Parameters>& params, float width, float height, int col, int row);
	std::unique_ptr<Quadratic> ShortestCollisionPoint(std::vector<std::shared_ptr<Sphere>> spheres);
	bool Collides(std::vector<std::shared_ptr<Sphere>> spheres);
	glm::vec4 Origin() const;
	glm::vec4 Direction() const;
	void SetDirection(glm::vec4 newDirection);
private:
	std::unique_ptr<Quadratic> GetCollisionPoint(std::shared_ptr<Sphere> sphere, bool hitOnly);
};
