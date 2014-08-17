#pragma once

#include <math.h>

class BoundingBox {
public:
	BoundingBox() {
		x0 = 0;
		x1 = 0;
		y0 = 0;
		y1 = 0;
		z0 = 0;
		z1 = 0;
	}

	BoundingBox(float p_x0, float p_x1, float p_y0, float p_y1, float p_z0, float p_z1) {
		x0 = p_x0;
		x1 = p_x1;
		y0 = p_y0;
		y1 = p_y1;
		z0 = p_z0;
		z1 = p_z1;
	}

	void adjustToInclude(BoundingBox* p_box) {
		x0 = min(x0, p_box->x0);
		x1 = max(x1, p_box->x1);
		y0 = min(y0, p_box->y0);
		y1 = max(y1, p_box->y1);
		z0 = min(z0, p_box->z0);
		z1 = max(z1, p_box->z1);
	}

	float x0, x1;
	float y0, y1;
	float z0, z1;

	bool intersects(BoundingBox* p_bbox) {
		return (x0 <= p_bbox->x1 && x1 >= p_bbox->x0 &&
			y0 <= p_bbox->y1 && y1 >= p_bbox->y0 &&
			z0 <= p_bbox->z1 && z1 >= p_bbox->z0);
	}

	Vector3d getCenter() {
		Vector3d c;
		c.x = (x1 - x0) / 2.0f;
		c.y = (y1 - y0) / 2.0f;
		c.z = (z1 - z0) / 2.0f;

		return c;
	}

	float getMaxLength() {
		float dx;
		float dy;
		float dz;

		dx = (x1 - x0);
		dy = (y1 - y0);
		dz = (z1 - z0);

		if(dx > dy && dx > dz) {
			return dx;
		} else if(dy > dz) {
			return dy;
		} 
		//else:
		return dz;
	}

private:
	float min(float a, float b) {
		return (a > b)? b: a;
	}

	float max(float a, float b) {
		return (a < b)? b: a;
	}
};


