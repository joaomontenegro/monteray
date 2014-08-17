#pragma once

#include "Image.h"

Image::Image() {
}

Image::Image(int p_width, int p_height) {
	width = p_width;
	height = p_height;

	init();
}

Image::~Image(void) {
	for(int i = 0; i < width; i++) {
		RGB* column = bitmap[i];
		delete column;
	}

	delete bitmap;
}

void Image::init() {
	bitmap = new RGB*[width];
	for(int i = 0; i < width; i++) {
		bitmap[i] = new RGB[height];
	}
}

void Image::putPixel(int p_x, int p_y, RGB p_rgb) {
	bitmap[p_x][p_y] = p_rgb;
}

RGB Image::getPixel(int p_x, int p_y) {
	return bitmap[p_x][p_y];
}

RGB Image::getTexel(float p_u, float p_v) {
	
	int x = (int)(p_u * width);
	int y = (int)(p_v * height);

	if(x < 0 || x >= width) {
		x = 0;
	}

	if(y < 0 || y >= height) {
		y = 0;
	}

	return bitmap[x][y];
}

void Image::writePPM(ostream& p_out) {
	p_out << "P6\n";
	p_out << width << ' ' << height << '\n';
	p_out <<"255\n";

	int i,j;
	unsigned int ired, igreen, iblue;
	unsigned char red, green, blue;
	
	RGB rgb;
	for(i = height - 1; i >= 0; i--) {
		for(j = 0; j < width; j++) {
			rgb = bitmap[j][i];
			rgb.r = (rgb.r > 1.0f)? 1.0f: rgb.r;
			rgb.g = (rgb.g > 1.0f)? 1.0f: rgb.g;
			rgb.b = (rgb.b > 1.0f)? 1.0f: rgb.b;
			rgb.r = (rgb.r < 0.0f)? 0.0f: rgb.r;
			rgb.g = (rgb.g < 0.0f)? 0.0f: rgb.g;
			rgb.b = (rgb.b < 0.0f)? 0.0f: rgb.b;
			ired = (unsigned int)(256*rgb.r);
			igreen = (unsigned int)(256*rgb.g);
			iblue = (unsigned int)(256*rgb.b);
			red = (unsigned char)(ired);
			green = (unsigned char)(igreen);
			blue = (unsigned char)(iblue);
			p_out.put(red);
			p_out.put(green);
			p_out.put(blue);
		}
	}
}

void Image::writeBMP(char* p_fileName) {
	BMP bmp;

	bmp.SetSize(width, height);
	bmp.SetBitDepth(24);
	int imgY = height - 1;
	RGB rgb;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			rgb = getPixel(x, imgY);
			rgb.r = (rgb.r > 1.0f)? 1.0f: rgb.r;
			rgb.g = (rgb.g > 1.0f)? 1.0f: rgb.g;
			rgb.b = (rgb.b > 1.0f)? 1.0f: rgb.b;
			rgb.r = (rgb.r < 0.0f)? 0.0f: rgb.r;
			rgb.g = (rgb.g < 0.0f)? 0.0f: rgb.g;
			rgb.b = (rgb.b < 0.0f)? 0.0f: rgb.b;
			bmp(x, y)->Red = (ebmpBYTE)(rgb.r * 255);
			bmp(x, y)->Green = (ebmpBYTE)(rgb.g * 255);
			bmp(x, y)->Blue = (ebmpBYTE)(rgb.b * 255);
			bmp(x, y)->Alpha = 0;
		}
		imgY--;
	}

	bmp.WriteToFile(p_fileName);
}

bool Image::readBMP(char* p_fileName) {
	BMP bmp;

	if(!bmp.ReadFromFile(p_fileName)) {
		printf("ERROR LOADING: %s\n", p_fileName);
		return false;
	}

	width = bmp.TellWidth();
	height = bmp.TellHeight();
	init();

	int imgY = height - 1;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			float r = ((float)bmp(x, y)->Red) / 255.0f;
			float g = ((float)bmp(x, y)->Green) / 255.0f;
			float b = ((float)bmp(x, y)->Blue) / 255.0f;
			RGB color(r, g, b);
				
			putPixel(x, imgY, color);
		}
		imgY--;
	}
	return true;
}