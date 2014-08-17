#pragma once

#include "Vector3d.h"
#include "BoundingBox.h"
#include "UVMap.h"

class Object3d;

struct RayIntersection {
	float dist;
	Vector3d point;
	Vector3d normal;
	UVMap uv;
	Object3d *object;
	bool hit;
};

class Ray {
public:
	Vector3d point;
	Vector3d direction;
	bool diffuse;

	Ray() {setPoint(Vector3d(0,0,0)); setDir(Vector3d(0,0,0)); diffuse = false;}
	Ray(Vector3d p_point, Vector3d p_direction) {setPoint(p_point); setDir(p_direction); diffuse = false;}

	void setPoint(Vector3d p_point) {point = p_point;}
	Vector3d getPoint() {return point;}
	
	void setDir(Vector3d p_direction) {direction = p_direction;}
	Vector3d getDir() {return direction;}

	void setDiffuse(bool p_diffuse) {diffuse = p_diffuse;}
	bool getDiffuse() {return diffuse;}

	Vector3d getPointAt(float p_dist);

	bool intersects(BoundingBox* p_box, float t0, float t1);

	bool intersects(Object3d** p_objList, int p_objCount, float p_maxDist);
	bool nearestIntersection(Object3d** p_objList, int p_objCount, RayIntersection* intersection);
	bool Ray::nearestIntersectionWithObject(Object3d* p_obj, RayIntersection* intersection);
};

