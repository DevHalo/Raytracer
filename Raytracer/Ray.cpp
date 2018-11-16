#include "pch.h"
#include "Ray.h"
#include "glm/vec3.hpp"

glm::vec3 a;
glm::vec3 c;

// r(t) = a + ct, t = {0, +inf}

int depth;

Ray::Ray()
{
	depth = 1;
}


Ray::~Ray()
{
}

// Sets the initial depth of the ray
void Ray::SetDepth(int d)
{
	depth = d;
}