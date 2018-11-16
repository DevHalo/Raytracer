#include "pch.h"
#include "catch.hpp"
#include "../Raytracer/FileReader.cpp"

// Unit test for verifying input and output of the raytracer

TEST_CASE("FileReader Test")
{
	FileReader reader;
	
	// If a file is not found, FileReader returns null pointer
	SECTION("File not found")
	{
		CHECK(reader.ReadFile("") == nullptr);
	}

	// If a file is found, FileReader will return a valid pointer
	// containing the data
	SECTION("testAmbient.txt IO Test")
	{
		std::shared_ptr<Parameters> params = reader.ReadFile("input/testAmbient.txt");
		// Null Test
		REQUIRE(params != nullptr);
		
		// Basic Parameter Test
		REQUIRE(*params->near == 1);
		REQUIRE(*params->left == -1);
		REQUIRE(*params->right == 1);
		REQUIRE(*params->bottom == -1);
		REQUIRE(*params->resX == 600);
		REQUIRE(*params->resY == 600);

		// Background Colour
		REQUIRE(params->back->r == 1);
		REQUIRE(params->back->g == 1);
		REQUIRE(params->back->b == 1);

		// Ambient Colour
		REQUIRE(params->ambient->r == 0.75f);
		REQUIRE(params->ambient->g == 0.75f);
		REQUIRE(params->ambient->b == 0.75f);

		// Sphere 1
		REQUIRE(params->spheres[0]->name == "s1");

		REQUIRE(params->spheres[0]->pos.x == 0);
		REQUIRE(params->spheres[0]->pos.y == 0);
		REQUIRE(params->spheres[0]->pos.z == -10);

		REQUIRE(params->spheres[0]->scl.x == 2);
		REQUIRE(params->spheres[0]->scl.y == 4);
		REQUIRE(params->spheres[0]->scl.z == 2);

		REQUIRE(params->spheres[0]->colour.r == 0.5f);
		REQUIRE(params->spheres[0]->colour.g == 0);
		REQUIRE(params->spheres[0]->colour.b == 0);

		REQUIRE(params->spheres[0]->k_a == 1);
		REQUIRE(params->spheres[0]->k_diff == 0);
		REQUIRE(params->spheres[0]->k_spec == 0);
		REQUIRE(params->spheres[0]->k_refl == 0);
		REQUIRE(params->spheres[0]->n == 50);

		// Sphere 2
		REQUIRE(params->spheres[1]->name == "s2");

		REQUIRE(params->spheres[1]->pos.x == 4);
		REQUIRE(params->spheres[1]->pos.y == 4);
		REQUIRE(params->spheres[1]->pos.z == -10);

		REQUIRE(params->spheres[1]->scl.x == 1);
		REQUIRE(params->spheres[1]->scl.y == 2);
		REQUIRE(params->spheres[1]->scl.z == 1);

		REQUIRE(params->spheres[1]->colour.r == 0);
		REQUIRE(params->spheres[1]->colour.g == 0.5f);
		REQUIRE(params->spheres[1]->colour.b == 0);

		REQUIRE(params->spheres[1]->k_a == 1);
		REQUIRE(params->spheres[1]->k_diff == 0);
		REQUIRE(params->spheres[1]->k_spec == 0);
		REQUIRE(params->spheres[1]->k_refl == 0);
		REQUIRE(params->spheres[1]->n == 50);

		// Sphere 3
		REQUIRE(params->spheres[2]->name == "s3");

		REQUIRE(params->spheres[2]->pos.x == -4);
		REQUIRE(params->spheres[2]->pos.y == 2);
		REQUIRE(params->spheres[2]->pos.z == -10);

		REQUIRE(params->spheres[2]->scl.x == 1);
		REQUIRE(params->spheres[2]->scl.y == 2);
		REQUIRE(params->spheres[2]->scl.z == 1);

		REQUIRE(params->spheres[2]->colour.r == 0);
		REQUIRE(params->spheres[2]->colour.g == 0);
		REQUIRE(params->spheres[2]->colour.b == 0.5f);

		REQUIRE(params->spheres[2]->k_a == 1);
		REQUIRE(params->spheres[2]->k_diff == 0);
		REQUIRE(params->spheres[2]->k_spec == 0);
		REQUIRE(params->spheres[2]->k_refl == 0);
		REQUIRE(params->spheres[2]->n == 50);

		// Light 1
		REQUIRE(params->lights[0]->name == "l1");

		REQUIRE(params->lights[0]->pos.x == 0);
		REQUIRE(params->lights[0]->pos.y == 0);
		REQUIRE(params->lights[0]->pos.z == 0);

		REQUIRE(params->lights[0]->intensity.r == 0.3f);
		REQUIRE(params->lights[0]->intensity.g == 0.3f);
		REQUIRE(params->lights[0]->intensity.b == 0.3f);

		// Light 2
		REQUIRE(params->lights[1]->name == "l2");

		REQUIRE(params->lights[1]->pos.x == 10);
		REQUIRE(params->lights[1]->pos.y == 10);
		REQUIRE(params->lights[1]->pos.z == -10);

		REQUIRE(params->lights[1]->intensity.r == 0.9f);
		REQUIRE(params->lights[1]->intensity.g == 0.9f);
		REQUIRE(params->lights[1]->intensity.b == 0);
		
		// Light 3
		REQUIRE(params->lights[2]->name == "l3");

		REQUIRE(params->lights[2]->pos.x == -10);
		REQUIRE(params->lights[2]->pos.y == 5);
		REQUIRE(params->lights[2]->pos.z == -5);

		REQUIRE(params->lights[2]->intensity.r == 0);
		REQUIRE(params->lights[2]->intensity.g == 0);
		REQUIRE(params->lights[2]->intensity.b == 0.9f);

		REQUIRE(params->output == "testAmbient.ppm");
	}

	SECTION("testParsing.txt IO Test")
	{
		std::shared_ptr<Parameters> params = reader.ReadFile("input/testParsing.txt");
		// Null Test
		REQUIRE(params != nullptr);

		// Basic Parameter Test
		REQUIRE(*params->near == 1);
		REQUIRE(*params->left == -1);
		REQUIRE(*params->right == 1);
		REQUIRE(*params->bottom == -1);
		REQUIRE(*params->resX == 600);
		REQUIRE(*params->resY == 600);

		// Background Colour
		REQUIRE(params->back->r == 1);
		REQUIRE(params->back->g == 1);
		REQUIRE(params->back->b == 1);

		// Ambient Colour
		REQUIRE(params->ambient->r == 0.2f);
		REQUIRE(params->ambient->g == 0.2f);
		REQUIRE(params->ambient->b == 0.2f);

		// Sphere 1
		REQUIRE(params->spheres[0]->name == "s1");

		REQUIRE(params->spheres[0]->pos.x == 0);
		REQUIRE(params->spheres[0]->pos.y == 0);
		REQUIRE(params->spheres[0]->pos.z == -10);

		REQUIRE(params->spheres[0]->scl.x == 2);
		REQUIRE(params->spheres[0]->scl.y == 4);
		REQUIRE(params->spheres[0]->scl.z == 2);

		REQUIRE(params->spheres[0]->colour.r == 0.5f);
		REQUIRE(params->spheres[0]->colour.g == 0);
		REQUIRE(params->spheres[0]->colour.b == 0);

		REQUIRE(params->spheres[0]->k_a == 1);
		REQUIRE(params->spheres[0]->k_diff == 1);
		REQUIRE(params->spheres[0]->k_spec == 0.9f);
		REQUIRE(params->spheres[0]->k_refl == 0);
		REQUIRE(params->spheres[0]->n == 50);

		// Sphere 2
		REQUIRE(params->spheres[1]->name == "s2");

		REQUIRE(params->spheres[1]->pos.x == 4);
		REQUIRE(params->spheres[1]->pos.y == 4);
		REQUIRE(params->spheres[1]->pos.z == -10);

		REQUIRE(params->spheres[1]->scl.x == 1);
		REQUIRE(params->spheres[1]->scl.y == 2);
		REQUIRE(params->spheres[1]->scl.z == 1);

		REQUIRE(params->spheres[1]->colour.r == 0);
		REQUIRE(params->spheres[1]->colour.g == 0.5f);
		REQUIRE(params->spheres[1]->colour.b == 0);

		REQUIRE(params->spheres[1]->k_a == 1);
		REQUIRE(params->spheres[1]->k_diff == 1);
		REQUIRE(params->spheres[1]->k_spec == 0.9f);
		REQUIRE(params->spheres[1]->k_refl == 0);
		REQUIRE(params->spheres[1]->n == 50);

		// Sphere 3
		REQUIRE(params->spheres[2]->name == "s3");

		REQUIRE(params->spheres[2]->pos.x == -4);
		REQUIRE(params->spheres[2]->pos.y == 2);
		REQUIRE(params->spheres[2]->pos.z == -10);

		REQUIRE(params->spheres[2]->scl.x == 1);
		REQUIRE(params->spheres[2]->scl.y == 2);
		REQUIRE(params->spheres[2]->scl.z == 1);

		REQUIRE(params->spheres[2]->colour.r == 0);
		REQUIRE(params->spheres[2]->colour.g == 0);
		REQUIRE(params->spheres[2]->colour.b == 0.5f);

		REQUIRE(params->spheres[2]->k_a == 1);
		REQUIRE(params->spheres[2]->k_diff == 1);
		REQUIRE(params->spheres[2]->k_spec == 0.9f);
		REQUIRE(params->spheres[2]->k_refl == 0);
		REQUIRE(params->spheres[2]->n == 50);

		// Light 1
		REQUIRE(params->lights[0]->name == "l1");

		REQUIRE(params->lights[0]->pos.x == 0);
		REQUIRE(params->lights[0]->pos.y == 0);
		REQUIRE(params->lights[0]->pos.z == 0);

		REQUIRE(params->lights[0]->intensity.r == 0.9f);
		REQUIRE(params->lights[0]->intensity.g == 0.9f);
		REQUIRE(params->lights[0]->intensity.b == 0.9f);

		// Light 2
		REQUIRE(params->lights[1]->name == "l2");

		REQUIRE(params->lights[1]->pos.x == 10);
		REQUIRE(params->lights[1]->pos.y == 10);
		REQUIRE(params->lights[1]->pos.z == -10);

		REQUIRE(params->lights[1]->intensity.r == 0.9f);
		REQUIRE(params->lights[1]->intensity.g == 0.9f);
		REQUIRE(params->lights[1]->intensity.b == 0);

		// Light 3
		REQUIRE(params->lights[2]->name == "l3");

		REQUIRE(params->lights[2]->pos.x == -10);
		REQUIRE(params->lights[2]->pos.y == 5);
		REQUIRE(params->lights[2]->pos.z == -5);

		REQUIRE(params->lights[2]->intensity.r == 0);
		REQUIRE(params->lights[2]->intensity.g == 0);
		REQUIRE(params->lights[2]->intensity.b == 0.9f);

		REQUIRE(params->output == "testParsing.ppm");
	}
}