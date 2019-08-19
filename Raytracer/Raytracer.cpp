#include "pch.h"
#include "FileReader.h"
#include "Ray.h"
#include "ppm.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include <future>
#include <iostream>

using namespace std;

unique_ptr<Parameters> params;		// Global reference to parameters
const int MAX_DEPTH = 4;			// Max recursive ray trace operations per ray
float height, width;				// Height and width of the camera space

glm::vec3 raytrace(Ray& ray)
{
	// If the ray has bounced more than 3 times, return black
	if (ray.Depth() > MAX_DEPTH) { return { 0.f, 0.f, 0.f }; }

	// Calculate the closest sphere that hits the ray
	std::unique_ptr<Quadratic> quad = ray.ShortestCollisionPoint(params->spheres);

	// If a ray hits a sphere, compute lighting
	if (quad)
	{
		glm::vec4 N, intersect;
		glm::vec3 colour{};
		
		// If the sphere is inside the near plane, flip normal
		// To calculate normal we must calculate the gradient which is
		// p/r^2 where p is the point on the ellipsoid and r is radius in x,y,z
		intersect = ray.Direction() * (quad->flip ? 
			glm::max(quad->t1, quad->t2) : 	
			glm::min(quad->t1, quad->t2)	
		);
		intersect.w = 1.f;

		N = intersect - quad->sphere->pos;
		N = N / (quad->sphere->scl * quad->sphere->scl);
		N.w = 0.f;
		N = normalize(quad->flip ? -N : N);

		// Calculate Phong illumination by iterating through all lights in the scene
		for (auto it = params->lights.begin(); it != params->lights.end(); ++it)
		{
			// Create a ray from the collision point to the light
			glm::vec4 L = (*it)->pos - intersect;
			L.w = 0.f;
			Ray sRay(intersect, L, 0);

			// If the ray does not collide with anything, apply L.I model
			if (!sRay.Collides(params->spheres))
			{
				L = normalize(L);
				glm::vec4 R = normalize(2.f * dot(L, N) * N - L);	// Reflection ray from light
				float dt = dot(intersect, intersect) - 1.f;
				glm::vec4 V = -intersect / sqrt(dt);				// Vector towards the camera

				// Compute dots separately to save computations
				float diffuseDot = dot(N, L);
				float specularDot = dot(R, V);

				// Formula is:
				// K_diffuse * Light Intensity * (N . L) * Object Colour +
				// K_specular * Light Intensity * (R . V)^n

				// Compute diffuse component
				if (diffuseDot > 0.f)
				{
					colour += quad->sphere->k_diff * (*it)->intensity * diffuseDot * quad->sphere->colour;
				}

				// Compute specular component
				if (specularDot > 0.f)
				{
					colour += quad->sphere->k_spec * (*it)->intensity * pow(specularDot, quad->sphere->n);
				}
			}
		}
		
		// If the reflective constant is 0, don't bother tracing reflections
		if (quad->sphere->k_refl != 0.f)
		{
			// Generate reflected ray
			auto reflRayDir = -2.f * dot(N, ray.Direction()) * N + ray.Direction();
			Ray reflRay = Ray( intersect, reflRayDir, ray.Depth() + 1);

			// Recursively raytrace
			colour += raytrace(reflRay) * quad->sphere->k_refl;
		}
	
		return (quad->sphere->k_a * *params->ambient * quad->sphere->colour) + colour;
	}
		
	// No collision, return background colour. If reflected ray, return black
	return ray.Depth() > 0 ? glm::vec3(0.f, 0.f, 0.f) : *params->back;
}

// Pre-calculates the inverse of the transformation matrix when used in
// sphere-line intersections
void CalculateSphereInverses()
{
	// Identity matrix
	const glm::mat4 identity = glm::mat4(1.f);

	for (auto it = params->spheres.begin(); it != params->spheres.end(); ++it)
	{
		// If the sphere is not canonical, we can pre-compute the transformation
		// matrix to reduce calculations per pixel for sphere-line intersections
		if ((*it)->scl.x != 1.f || (*it)->scl.y != 1.f || (*it)->scl.z != 1.f)
		{
			// Transformation matrix M = Translation * Rotation * Scale
			(*it)->transformMatrix = 
				translate(identity, glm::vec3((*it)->pos)) * 
				scale(identity, glm::vec3((*it)->scl));

			// Invert the transformation matrix
			(*it)->inverseMatrix = inverse((*it)->transformMatrix);
		}
		else
		{
			// No changes required, just use identity matrix;
			(*it)->transformMatrix = identity;
			(*it)->inverseMatrix = identity;
		}
	}
}

bool WorkerRaytrace(int startRow, int endRow, std::vector<unsigned char>& pixels)
{
	int scaler = (*params->resY - 1) * *params->resX;
	for (int row = startRow; row < endRow; row++)
	{
		int index = (scaler - row * *params->resX) * 3;
		for (int col = 0; col < *params->resX; col++)
		{
			Ray pRay = Ray(params, width, height, col, row);

			glm::vec3 objectColour = raytrace(pRay);
			pixels[index] = glm::min(255.f, objectColour.r * 255.f);
			pixels[index + 1] = glm::min(255.f, objectColour.g * 255.f);
			pixels[index + 2] = glm::min(255.f, objectColour.b * 255.f);
			index += 3;
		}
	}

	return true;
}

// Entry point for raytracing application
int main(const int argc, char* arg[])
{
	// If the number of arguments is not 1
	if (argc == 1)
	{
		cout << "Invalid argument count\nUsage: raytracer.exe <filename> -params\nFilename must be .txt\n" <<
			"-multi : Take advantage of multithreaded rendering (In this case all cores will be used)";
		return -1;
	}

	cout << "Reading file... ";

	params = FileReader::ReadFile(arg[1]);

	// If the file was not successfully read
	if (params == nullptr)
	{
		cout << "Failed to read file \"" << arg[1] << "\"";
		return -1;
	}

	cout << "Done.\n";
	cout << "NOTE: Parameters LEFT, RIGHT, TOP, and BOTTOM are deprecated. They have no effect on the result\n";

	bool runAsync = false;
	unsigned int threads = 0;
	if (arg[2] && std::string(arg[2]) == "-multi")
	{
		cout << "Multithreaded Rendering was selected.\nDetecting number of threads...\n";
		threads = std::thread::hardware_concurrency();

		if (threads == 0) { cout << "Unable to detect number of threads. Defaulting to 1.\n"; }
		else
		{
			cout << "Detected " << threads << " threads.\n";
			runAsync = true;
		}
	}

	cout << "Pre-calculating Sphere Matrices... ";

	// Pre-calculate the inverse matrix for spheres
	CalculateSphereInverses();

	cout << "Done.\n";

	// Store our array of pixels row, column
	auto pixels = std::vector<unsigned char>();
	pixels.reserve(*params->resX * *params->resY * 3);
	
	width = ((float(*params->resX) / *params->resY) + 1.f) / 2.f;	
	height = ((float(*params->resY) / *params->resX) + 1.f) / 2.f;

	cout << "Raytracing " << *params->resX << " x " << *params->resY << " pixels...";

	// Used for timing
	auto start = std::chrono::high_resolution_clock::now();

	if (runAsync)
	{
		auto activeThreads = std::vector<std::future<bool>>();

		// Thread count is divided by 2 to divide the image into equal parts
		int rowSize = *params->resY / threads;
		int rowRemainder = *params->resY - (*params->resY % threads);

		// Iterate through each chunk and assign a thread for it
		for (int row = 0; row + rowSize <= rowRemainder; row += rowSize)
		{
			activeThreads.push_back(std::async(std::launch::async,
				WorkerRaytrace, row, row + rowSize, std::ref(pixels))
			);
		}

		// Process any remaining pixels
		if (rowRemainder != *params->resY)
		{
			activeThreads.push_back(std::async(std::launch::async,
				WorkerRaytrace, rowRemainder, *params->resY - 1, std::ref(pixels))
			);
		}

		// Run through all threads and make sure they have completed
		for (auto it = activeThreads.begin(); it != activeThreads.end(); ++it) { (*it).get(); }
	}
	else
	{
		// Sequential raytracing loop
		int index = 0;
		for (int row = *params->resY - 1; row >= 0; row--)
		{
			for (int col = 0; col < *params->resX; col++)
			{
				// Get colour from primary ray and subsequent raytraces
				Ray pRay = Ray(params, width, height, col, row);

				glm::vec3 objectColour = raytrace(pRay);
				pixels[index] = glm::min(255.f, objectColour.r * 255.f);
				pixels[index + 1] = glm::min(255.f, objectColour.g * 255.f);
				pixels[index + 2] = glm::min(255.f, objectColour.b * 255.f);
				index += 3;
			}
		}
	}

	long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now() - start).count();

	cout << " Done.\n";

	// Cast string to c-style string and save file to ppm
	const char* out = params->output.c_str();
	ppm::save_imageP6(*params->resX, *params->resY, out, &pixels[0]);

	cout << " Done.\nRaytracing finished.\nRendering Time: " << elapsed << "milliseconds\n";

	return 0;
}
