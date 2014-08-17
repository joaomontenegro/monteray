#pragma once

#include "Ray.h"

class Camera {
public:
	Camera(int p_width, int p_height, Vector3d p_position) {width = p_width; height = p_height; position = p_position;}

	int getHeight() {return height;}
	void setHeight(int p_heihgt) {height = p_heihgt;}
	
	int getWidth() {return width;}
	void setWidth(int p_width) {width = p_width;}

	Vector3d getPosition() {return position;}
	void setPosition(Vector3d p_position) {position = p_position;}

	virtual Ray castRay(float x, float y) = 0;

protected:
	int width;
	int height;
	Vector3d position;
};