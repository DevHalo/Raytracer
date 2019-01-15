#include "pch.h"
#include "FileReader.h"
#include "Ray.h"
#include "ppm.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include <ctime>
#include <future>
#include <iostream>

using namespace std;

unique_ptr<Parameters> params;
const int MAX_DEPTH = 4;
float height, width;

glm::vec3 raytrace(Ray& ray)
{
	// If the ray has bounced more than 3 times, return black
	if (ray.Depth() > MAX_DEPTH) { return { 0.f, 0.f, 0.f }; }

	glm::vec3 cShadow{}, cRefl{};

	// Calculate the closest sphere that hits the ray
	std::unique_ptr<Quadratic> quad = ray.ShortestCollisionPoint(params->spheres);

	// If a ray hits a sphere, compute lighting
	if (quad)
	{
		// Ambient Component
		glm::vec3 cAmbient = quad->sphere->k_a * *params->ambient * quad->sphere->colour;
		
		float t;
		glm::vec4 N, intersect, reflRayDir;
		
		// If the sphere is inside the near plane, flip normal
		// To calculate normal we must calculate the gradient which is
		// p/r^2 where p is the point on the ellipsoid and r is radius in x,y,z
		if (quad->flip)
		{
			t = glm::max(quad->t1, quad->t2);
			intersect = ray.Direction() * t;
			intersect.w = 1.f;

			N = intersect - quad->sphere->pos;
			N = N / (quad->sphere->scl * quad->sphere->scl);
			N.w = 0.f;
			N = normalize(-N);
		}
		else
		{
			t = quad->t1 < quad->t2 ? quad->t1 : quad->t2;
			intersect = ray.Direction() * t;
			intersect.w = 1.f;
			
			N = intersect - quad->sphere->pos;
			N = N / (quad->sphere->scl * quad->sphere->scl);
			N.w = 0.f;
			N = normalize(N);
			
			/*
			N = transpose(quad->sphere->inverseMatrix) * (intersect - quad->sphere->pos);
			N.w = 0.f;
			N = normalize(N);
			*/
		}

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
					cShadow += quad->sphere->k_diff * (*it)->intensity * diffuseDot * quad->sphere->colour;
				}

				// Compute specular component
				if (specularDot > 0.f)
				{
					cShadow += quad->sphere->k_spec * (*it)->intensity * pow(specularDot, quad->sphere->n);
				}
			}
		}
		
		// If the reflective constant is 0, don't bother tracing reflections
		if (quad->sphere->k_refl != 0.f)
		{
			// Calculate reflected ray
			reflRayDir = -2.f * dot(N, ray.Direction()) * N + ray.Direction();
			Ray reflRay = Ray(intersect, reflRayDir, ray.Depth() + 1);

			// Recursively raytrace
			cRefl = raytrace(reflRay) * quad->sphere->k_refl;
		}

		// Sum up each component
		glm::vec3 cTotal = cAmbient + cShadow + cRefl;

		// Clamp the total if the values are over 1.0f
		return cTotal;
	}
		
	// No collision, return background colour
	// If reflected ray, return black
	return ray.Depth() > 0 ? glm::vec3(0.f, 0.f, 0.f) : *params->back;
}

// Pre-calculates the inverse of the transformation matrix when used in
// sphere-line intersections
void CalculateSphereInverses()
{
	for (auto it = params->spheres.begin(); it != params->spheres.end(); ++it)
	{
		// Identity matrix
		glm::mat4 identity = glm::mat4(1.f);

		// If the sphere is not canonical, we can pre-compute the transformation
		// matrix to reduce calculations per pixel for sphere-line intersections
		if ((*it)->scl.x != 1.f || (*it)->scl.y != 1.f || (*it)->scl.z != 1.f)
		{
			// Translation matrix
			glm::mat4 transform = translate(identity, glm::vec3((*it)->pos));

			// Scale matrix
			glm::mat4 scale = glm::scale(identity, glm::vec3((*it)->scl));

			// Transformation matrix M = Translation * Rotation * Scale
			(*it)->transformMatrix = transform * scale;

			// Invert the transformation matrix
			(*it)->inverseMatrix = inverse((*it)->transformMatrix);

			// Adjust position of sphere with the transformation matrix
			//(*it)->pos = (*it)->inverseMatrix * (*it)->pos;
		}
		else
		{
			// No changes required, just use identity matrix;
			(*it)->transformMatrix = identity;
			(*it)->inverseMatrix = identity;
		}
	}
}

bool Worker_Raytrace(int startCol, int endCol, int startRow, int endRow,
	std::vector<std::vector<glm::vec3>>& dirtyArray)
{
	for (int col = startCol; col <= endCol; col++)
	{
		for (int row = startRow; row <= endRow; row++)
		{
			Ray pRay = Ray(params, width, height, col, row);
			dirtyArray[col][row] = raytrace(pRay);
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

	FileReader reader;
	params = reader.ReadFile(arg[1]);

	// If the file was not successfully read
	if (params == nullptr)
	{
		cout << "Failed to read file \"" << arg[1] << "\"";
		return -1;
	}

	cout << "Done.\n";

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
	std::vector<unsigned char> pixels =
		std::vector<unsigned char>(*params->resX * *params->resY * 3);

	width = (*params->right - *params->left) / 2.f;
	height = (*params->top - *params->bottom) / 2.f;

	cout << "Raytracing " << *params->resX << " x " << *params->resY << " pixels...";
	int index = 0;

	// Timing variables
	auto start = std::chrono::high_resolution_clock::now();

	if (runAsync)
	{
		// Allocate 2d array of vectors. Does not require a mutex because
		// two threads will never write to the same memory location/index
		std::vector<std::vector<glm::vec3>> dirtyPixels;
		dirtyPixels.resize(*params->resX, std::vector<glm::vec3>(*params->resY));

		auto activeThreads = std::vector<std::future<bool>>();

		// Thread count is divided by 2 to divide the image into equal parts
		int rowSize = *params->resY / (threads / 2);
		int colSize = *params->resX / (threads / 2);
		int rowRemainder = *params->resY % (threads / 2);
		int colRemainder = *params->resX % (threads / 2);

		int index = 0;
		// Iterate through each chunk and assign a thread for it
		for (int row = 0; row + rowSize <= *params->resY; row += rowSize)
		{
			for (int col = 0; col + colSize <= *params->resX; col += colSize)
			{
				activeThreads.push_back(std::async(std::launch::async,
					Worker_Raytrace,
					col, col + colSize - 1,
					row, row + rowSize - 1,
					std::ref(dirtyPixels))
				);
			}

			// Process any remaining pixels inside a square that does not
			// match the block size
			if (colRemainder != 0)
			{
				activeThreads.push_back(std::async(std::launch::async,
					Worker_Raytrace,
					*params->resX - colRemainder, *params->resX - 1,
					row, row + rowSize - 1,
					std::ref(dirtyPixels))
				);
			}
		}

		// Process any remaining pixels outside the row
		if (rowRemainder != 0)
		{
			activeThreads.push_back(std::async(std::launch::async,
				Worker_Raytrace,
				0, *params->resX - 1,
				*params->resY - rowRemainder, *params->resY - 1,
				std::ref(dirtyPixels))
			);
		}

		// Run through all threads and make sure they have completed
		for (auto it = activeThreads.begin(); it != activeThreads.end(); ++it) { (*it).get(); }

		index = 0;
		// Copy each vector to the output array of unsigned chars
		for (int row = *params->resY - 1; row >= 0; row--)
		{
			for (int col = 0; col < *params->resX; col++)
			{
				glm::vec3 result = dirtyPixels[col][row];
				pixels[index] = glm::min(255.f, result.r * 255.f);
				pixels[index + 1] = glm::min(255.f, result.g * 255.f);
				pixels[index + 2] = glm::min(255.f, result.b * 255.f);
				index += 3;
			}
		}
	}
	else
	{
		// Sequential raytracing loop
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

	auto elapsed = std::chrono::high_resolution_clock::now() - start;

	cout << " Done.\n";

	// Cast string to c-style string and save file to ppm
	const char* out = params->output.c_str();
	ppm::save_imageP6(*params->resX, *params->resY, out, &pixels[0]);

	cout << " Done.\nRaytracing finished.\n";

	cout << "Rendering Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() 
		<< " milliseconds" << endl;

	return 0;
}
