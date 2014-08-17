#pragma once

#include "Object3d.h"
#include "UVMap.h"
#include <math.h>
#include <stdio.h>

class Sphere : public Object3d {
public:
	Sphere(Vector3d p_center, float p_radius, Shader* p_shader);

	void setCenter(Vector3d p_center) {center = p_center;}
	Vector3d getCenter() {return center;}

	void setRadius(float p_radius) {radius = p_radius;}
	float getRadius() {return radius;}

	UVMap Sphere::uvMapAt(Vector3d p_point);

	void initBoundingBox();

	bool isIntersectedBy(Ray* p_ray, float p_maxDist);
	bool nearestIntersection(Ray* p_ray, RayIntersection* intersection);

private:
	Vector3d center;
	float radius;
};