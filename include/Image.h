#pragma once

#include "RGB.h"
#include "EasyBMP.h"
#include <stdio.h>
#include <iostream>

using namespace std;

class Image
{
public:
	Image();
	Image(int p_width, int p_height);
	~Image(void);

	void init();

	int getWidth() {return width;}
	int getHeight() {return height;}

	void putPixel(int p_x, int p_y, const RGB rgb);
	RGB getPixel(int p_x, int p_y);
	
	RGB getTexel(float p_u, float p_v);

	void writePPM(ostream& p_out);

	void writeBMP(char* p_fileName);

	bool readBMP(char* p_fileName);

private:
	int width;
	int height;

	RGB** bitmap;
};

