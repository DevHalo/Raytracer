#pragma once

// Container for holding an RGB value
struct Colour
{
	float r, g, b;

	Colour(float _r, float _g, float _b) :
		r(_r), g(_g), b(_b) {};
};
