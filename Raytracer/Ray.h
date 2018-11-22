#pragma once
#include "FileReader.h"

class Ray
{
public:
	Ray(glm::vec4 origin, glm::vec4 direction);
	Ray(std::unique_ptr<Parameters>& params, float width, float height, int col, int row);
	std::shared_ptr<Sphere> SphereIntersect(std::vector<std::shared_ptr<Sphere>>& spheres) const;
};

