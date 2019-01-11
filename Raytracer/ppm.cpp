#include "pch.h"
#include "ppm.h"
#include <cstdio>

// Output in P6 format, a binary file containing:
// P6
// ncolumns nrows
// Max colour value
// colours in binary format thus unreadable
void ppm::save_imageP6(int Width, int Height, const char* fname, unsigned char* pixels) {
	FILE *fp;
	const int maxVal = 255;

	printf("Saving image %s: %d x %d", fname, Width, Height);
	fopen_s(&fp, fname, "wb");
	if (!fp) {
		printf("\nUnable to open file '%s'", fname);
		return;
	}
	fprintf(fp, "P6\n");
	fprintf(fp, "%d %d\n", Width, Height);
	fprintf(fp, "%d\n", maxVal);

	for (int j = 0; j < Height; j++) {
		fwrite(&pixels[j*Width * 3], 3, Width, fp);
	}

	fclose(fp);
}