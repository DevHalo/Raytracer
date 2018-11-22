#pragma once
class ppm
{
public:
	static void save_imageP6(int Width, int Height, const char* fname, unsigned char* pixels);
	static void save_imageP3(int Width, int Height, const char* fname, unsigned char* pixels);
};

