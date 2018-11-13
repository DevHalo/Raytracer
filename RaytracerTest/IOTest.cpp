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
		CHECK(reader.ReadFile(" ") == nullptr);
	}

	// If a file is found, FileReader will return a valid pointer
	// containing the data
	SECTION("Valid File")
	{
		CHECK(reader.ReadFile("input/testAmbient.txt") != nullptr);
	}
}