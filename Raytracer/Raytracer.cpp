#include "pch.h"
#include <iostream>
#include "FileReader.h"
#include "Colour.h"

using namespace std;

// Entry point for raytracing application
int main(int argc, char* arg[])
{
	// If the number of arguments is not 1
	if (argc != 2)
	{
		cout << "Invalid argument count" << endl << "Usage: raytracer.exe <filename> : filename must be .txt";
		return -1;
	}
	
	FileReader reader;
	reader.ReadFile(arg[0]);

    std::cout << "Hello World!\n"; 
}