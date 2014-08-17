#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

class RGB {
public:
	RGB(void) {r = g = b = 0;}

	RGB(float p_r, float p_g, float p_b) {
		r = p_r;
		g = p_g;
		b = p_b;
		a = 1.0;
	}

	float r; //red value
	float g; //green value
	float b; //blue value
	float a; //alpha value

	void setR(float p_r) {r = p_r;}
	void setG(float p_g) {g = p_g;}
	void setB(float p_b) {b = p_b;}
	void setA(float p_a) {a = p_a;}

	void setValues(float p_r, float p_g, float p_b, float p_a) {
		r = p_r;
		g = p_g;
		b = p_b;
		a = p_a;
	}

	void setValues(float p_r, float p_g, float p_b) {
		r = p_r;
		g = p_g;
		b = p_b;
		a = 1;
	}

	float getR() {return r;}
	float getG() {return g;}
	float getB() {return b;}

	float brightness();
	void randomize();
	void clamp();

	bool isZero() {return (r == 0.0 && g == 0.0 && b == 0.0);};

	//operators:
	friend RGB operator+(const RGB p_rgb1, const RGB p_rgb2);
	friend RGB operator*(const RGB p_rgb1, const RGB p_rgb2);
	friend RGB operator/(const RGB p_rgb1, const RGB p_rgb2);

	friend RGB operator*(float p_f, const RGB p_rgb1);
	friend RGB operator/(float p_f, const RGB p_rgb1);
	friend RGB operator*(const RGB p_rgb1, float p_f);
	friend RGB operator/(const RGB p_rgb1, float p_f);
};

inline float RGB::brightness() {
	return (r + g + b) / 3;
}

inline void RGB::randomize() {
	r = ((float)((rand() % 100000) + 1)) / 100000;
	g = ((float)((rand() % 100000) + 1)) / 100000;
	b = ((float)((rand() % 100000) + 1)) / 100000;

	r = sqrt(r);
	g = sqrt(g);
	b = sqrt(b);

}

inline void RGB::clamp() {
	if(r > 1) {r = 1;}
	if(g > 1) {g = 1;}
	if(b > 1) {b = 1;}

	if(r < 0) {r = 0;}
	if(g < 0) {g = 0;}
	if(b < 0) {b = 0;}
}

inline RGB operator+(const RGB p_rgb1, const RGB p_rgb2) {
	RGB rgb(p_rgb1.r + p_rgb2.r, p_rgb1.g + p_rgb2.g, p_rgb1.b + p_rgb2.b);
	return rgb;
}

inline RGB operator*(const RGB p_rgb1, const RGB p_rgb2){
	RGB rgb(p_rgb1.r * p_rgb2.r, p_rgb1.g * p_rgb2.g, p_rgb1.b * p_rgb2.b);
	return rgb;
}

inline RGB operator/(const RGB p_rgb1, const RGB p_rgb2){
	RGB rgb(p_rgb1.r / p_rgb2.r, p_rgb1.g / p_rgb2.g, p_rgb1.b / p_rgb2.b);
	return rgb;
}

inline RGB operator*(float p_f, const RGB p_rgb1){
	RGB rgb(p_rgb1.r * p_f, p_rgb1.g * p_f, p_rgb1.b * p_f);
	return rgb;
}

inline RGB operator/(float p_f, const RGB p_rgb1){
	RGB rgb(p_rgb1.r / p_f, p_rgb1.g / p_f, p_rgb1.b / p_f);
	return rgb;
}

inline RGB operator*(const RGB p_rgb1, float p_f){
	return p_f * p_rgb1;
}

inline RGB operator/(const RGB p_rgb1, float p_f){
	return p_f / p_rgb1;
}