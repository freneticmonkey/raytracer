// sourced from
// http://www.codermind.com/articles/Raytracer-in-C++-Part-III-Textures.html

#ifndef SDL2Test_perlin_h
#define SDL2Test_perlin_h

namespace perlinnoise
{

	double noise(double x, double y, double z);

	inline float noisef(float x, float y, float z)
	{
		return (float) noise((double)x, (double)y, (double)z);
	}
}

#endif