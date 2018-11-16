#include "pch.h"
#include "Ray.h"

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