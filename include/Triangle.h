#pragma once

#include "Vector3d.h"
#include "Object3d.h"
#include "UVMap.h"
#include <math.h>
#include <stdio.h>

class Triangle : public Object3d {
public:
	Triangle();
	Triangle(Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, Shader* p_shader);
	Triangle(Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, UVMap p_uv0, UVMap p_uv1, UVMap p_uv2, Shader* p_shader);
	Triangle(Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, Vector3d p_normal0, Vector3d p_normal1, Vector3d p_normal2, Shader* p_shader);
	Triangle(Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, Vector3d p_normal0, Vector3d p_normal1, Vector3d p_normal2, UVMap p_uv0, UVMap p_uv1, UVMap p_uv2, Shader* p_shader);

	void initBoundingBox();

	bool isIntersectedBy(Ray* p_ray, float p_maxDist);
	bool nearestIntersection(Ray* p_ray, RayIntersection* intersection);

	bool singleNormal;

	Vector3d p0;
	Vector3d p1;
	Vector3d p2;

	UVMap uv0;
	UVMap uv1;
	UVMap uv2;
	
	Vector3d v0;
	Vector3d v1;

	Vector3d normal;

	Vector3d normal0;
	Vector3d normal1;
	Vector3d normal2;
};
