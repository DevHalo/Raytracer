// RaytracerTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define CATCH_CONFIG_MAIN
#include "include/catch.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello World!\n"; 
}

TEST_CASE("Test1")
{
	REQUIRE(1 == 1);
}