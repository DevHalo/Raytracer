#include "pch.h"
#include "catch.hpp"
#include "../Raytracer/FileReader.cpp"

// Unit test for verifying input and output of the raytracer

TEST_CASE("asd")
{
	FileReader reader;

	
	SECTION("File not found")
	{
		WARN("FileReader Test Suite");
		CHECK(reader.ReadFile(" ") == nullptr);
	};
}