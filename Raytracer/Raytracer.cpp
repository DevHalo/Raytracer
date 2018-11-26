#include "pch.h"
#include <iostream>
#include "FileReader.h"
#include "Ray.h"
#include "ppm.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include <ctime>

using namespace std;

unique_ptr<Parameters> params;
const int MAX_DEPTH = 3;
int collisions = 0;

glm::vec3 raytrace(Ray ray)
{
	// If the ray has bounced more than 3 times, return black
	if (ray.Depth() > MAX_DEPTH) { return { 0.f, 0.f, 0.f }; }

	// Formula for colour is:
	// Local Illumination (Ambient * Diffuse)
	// Sum of light contribution:
	// - If intersect, Use light + diffuse + specular
	// - No 
	// Reflections (From collision point recursively raytrace)
	glm::vec3 cAmbient, cShadow{}, cRefl{};
	
	bool inside = false;

	// CLAMP LIGHTING VALUES FROM LIGHTS UNIFORMLY
	// CLAMP FROM THE LARGEST VALUE [11, 1, 1] => [1, 1/11, 1/11]
	// OR ELSE LIGHTING WILL LOOK WEIRD

	std::unique_ptr<Quadratic> quad = ray.ShortestCollisionPoint(params->spheres);

	if (quad)
	{
		// Ambient * Sphere colour
		cAmbient = quad->sphere->k_a * *params->ambient * quad->sphere->colour;
		
		// Calculate normal
		float t;
		glm::vec4 normal, intersect, reflRayDir;
		

		// If the sphere is inside the near plane, flip normal
		if (quad->t1 < 1.f)
		{
			t = quad->t2;
			intersect = (ray.Direction() * t) / quad->sphere->scl * quad->sphere->scl;
			intersect.w = 1.f;

			normal = -normalize(intersect - quad->sphere->pos);
		}
		else
		{
			t = quad->t1 < quad->t2 ? quad->t1 : quad->t2;
			intersect = ray.Direction() * t / quad->sphere->scl * quad->sphere->scl;

			normal = normalize(intersect - quad->sphere->pos);
		}
		

		// Calculate reflected ray
		reflRayDir = -2.f * dot(normal, ray.Direction()) + ray.Direction();
		Ray reflRay = Ray(intersect, reflRayDir, ray.Depth() + 1);

		/*
		for (auto it = params->lights.begin(); it != params->lights.end(); ++it)
		{
			// Create a ray from the collision point to the light
			glm::vec4 dir = (*it)->pos - intersect;
			Ray sRay(intersect, dir, 0);

			// If the ray does not collide with anything, apply L.I model
			if (!sRay.Collides(params->spheres))
			{
				cShadow +=
					quad->sphere->k_diff * (*it)->intensity * dot(normal, dir) * quad->sphere->colour +
					(quad->sphere->k_spec * (*it)->intensity * pow(dot(reflRayDir, -intersect), quad->sphere->n));

				Ray::ClampColour(cShadow);
			}
		}

		*/

		// If the reflective constant 0, don't bother tracing reflections
		//if (quad->sphere->k_refl != 0.f) { cRefl = raytrace(reflRay) * quad->sphere->k_refl; }
		
		glm::vec3 cTotal = cAmbient + cShadow + cRefl;
		Ray::ClampColour(cTotal);
		
		collisions++;
		return cTotal;
	}
		
	// No collision, return background colour
	return *params->back;
}

// Pre-calculates the inverse of the transformation matrix when used in
// sphere-line intersections
void CalculateSphereInverses()
{
	for (auto it = params->spheres.begin(); it != params->spheres.end(); ++it)
	{
		// If the sphere is not canonical, we can pre-compute the transformation
		// matrix to reduce calculations per pixel for sphere-line intersections
		if ((*it)->scl.x != 1.f || (*it)->scl.y != 1.f || (*it)->scl.z != 1.f)
		{
			// Identity matrix
			glm::mat4 identity = glm::mat4(1.f);

			// Translation matrix
			glm::mat4 transform = translate(identity, glm::vec3((*it)->pos));

			// Scale matrix
			glm::mat4 scale = glm::scale(identity, glm::vec3((*it)->scl));

			// Transformation matrix M = Translation * Rotation * Scale
			(*it)->transformMatrix = transform * scale;

			// Invert the transformation matrix
			(*it)->inverseMatrix = inverse((*it)->transformMatrix);

			// Adjust position of sphere with the transformation matrix
			(*it)->pos = (*it)->inverseMatrix * (*it)->pos;
		}
	}
}

// Entry point for raytracing application
int main(const int argc, char* arg[])
{
	// If the number of arguments is not 1
	if (argc != 2)
	{
		cout << "Invalid argument count" << endl << "Usage: raytracer.exe <filename> : filename must be .txt";
		return -1;
	}

	cout << "Reading file... ";

	FileReader reader;
	params = reader.ReadFile(arg[1]);

	// If the file was not successfully read
	if (params == nullptr)
	{
		cout << "Failed to read file \"" << arg[1] << "\"";
		return -1;
	}

	cout << "Done.\nPre-calculating Sphere Matrices... ";

	// Pre-calculate the inverse matrix for spheres
	CalculateSphereInverses();

	cout << "Done.\n";
	// Store our array of pixels row, column
	std::vector<unsigned char> pixels =
		std::vector<unsigned char>(*params->resX * *params->resY * 3);

	float width = (*params->right - *params->left) / 2.f;
	float height = (*params->top - *params->bottom) / 2.f;

	std::clock_t start;
	double duration;

	start = std::clock();

	cout << "Raytracing " << *params->resX << " x " << *params->resY << " pixels...";
	int index = 0;
	for (int row = *params->resY - 1; row >= 0; row--)
	{
		for (int col = 0; col < *params->resX; col++)
		{
			// Get colour from primary ray and subsequent raytraces
			Ray pRay = Ray(params, width, height, col, row);
			glm::vec3 objectColour = raytrace(pRay);

			pixels[index] = (objectColour.r * 255.f);
			pixels[index + 1] = (objectColour.g * 255.f);
			pixels[index + 2] = (objectColour.b * 255.f);
			index += 3;
		}
	}

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

	cout << " Done.\n";

	// Cast string to c-style string and save file to ppm
	const char* out = params->output.c_str();
	ppm::save_imageP6(*params->resX, *params->resY, out, &pixels[0]);

	cout << " Done.\nRaytracing finished.\n";

	cout << "Rendering Time: " << duration << endl;
	cout << "Collisions: " << collisions;

	return 0;
}
