#pragma once
class ppm
{
public:
	void save_imageP6(int Width, int Height, char* fname, unsigned char* pixels);
	void save_imageP3(int Width, int Height, char* fname, unsigned char* pixels);
};

