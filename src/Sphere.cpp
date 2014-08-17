#pragma once

#include "Sphere.h"
#include "Shader.h"

Sphere::Sphere(Vector3d p_center, float p_radius, Shader* p_shader) : Object3d(p_shader) {
	setName("sphere"); 
	setCenter(p_center); 
	setRadius(p_radius);
	initBoundingBox();
}

UVMap Sphere::uvMapAt(Vector3d p_point) {
	UVMap uv;
	const float PI = 3.14159265358979323846f;
	
	float theta = acosf((p_point.z - center.z) / radius);
	float phi = atan2f(p_point.y - center.y, p_point.x - center.x);

	if(phi < 0) {
		phi += 2 * PI;
	}

	uv.u = phi / (2*PI);
	uv.v = (PI - theta) / PI;

	//printf("-> %f %f | %f %f \n", theta, phi, uv.u, uv.v);

	if(uv.u < 0) {uv.u = -uv.u;}

	if(uv.v < 0) {uv.v = -uv.v;}

	return uv;
}

void Sphere::initBoundingBox() {
	boundingBox.x0 = center.x - radius;
	boundingBox.x1 = center.x + radius;
	boundingBox.y0 = center.y - radius;
	boundingBox.y1 = center.y + radius;
	boundingBox.z0 = center.z - radius;
	boundingBox.z1 = center.z + radius;
}

bool Sphere::isIntersectedBy(Ray* p_ray, float p_maxDist) {
	Vector3d p = p_ray->getPoint() - center;

	float b = 2 * p_ray->getDir().dot(p);
	float c = p.length2() - (radius * radius);
	float d = b * b - 4 * c;

	if(d < 0) {
		return false;
	}

	float dsqrt = sqrt(d);

	if(dsqrt == 0.0) {
		return true;
	}

	float t0 = (-b - dsqrt) / 2;
	if(t0 > EPSILON && t0 < p_maxDist + EPSILON) {
		return true;
	}

	float t1 = (-b + dsqrt) / 2;
	if(t1 > EPSILON && t1 < p_maxDist + EPSILON) {
		return true;
	}

	return false;	
}

bool Sphere::nearestIntersection(Ray* p_ray, RayIntersection* intersection) {
	Vector3d p = p_ray->getPoint() - center;
	float t;

	float b = 2 * p_ray->getDir().dot(p);
	float c = p.length2() - (radius * radius);
	float d = b * b - 4 * c;

	if(d < 0) {
		return false;
	}

	float dsqrt = sqrt(d);

	if(dsqrt == 0.0) {
		t = -b / 2;
		intersection->dist = t;
		Vector3d point = p_ray->getPointAt(t);
		Vector3d normal = point - center;
		normal.normalize();
		intersection->normal = normal;
		intersection->point = point;
		return true;
	}

	float t0 = (-b - dsqrt) / 2;
	float t1 = (-b + dsqrt) / 2;

	if(t0 < EPSILON && t1 < EPSILON) {
		return false;
	} else if(t0 < EPSILON) {
		t = t1;
	} else if(t1 < EPSILON) {
		t = t0;
	} else if(t0 < t1){
		t = t0;
	} else {
		t = t1;
	}

	intersection->dist = t;
	Vector3d point = p_ray->getPointAt(t);
	Vector3d normal = point - center;
	normal.normalize();
	intersection->normal = normal;
	intersection->point = point;

	intersection->uv = uvMapAt(point);
	intersection->object = this;

	return true;
}