#pragma once

#include "Triangle.h"
#include "Shader.h"

Triangle::Triangle() : Object3d(NULL) {
}

Triangle::Triangle(Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, Shader* p_shader) : Object3d(p_shader) {
	setName("triangle"); 
	p0 = p_p0;
	p1 = p_p1;
	p2 = p_p2;

	uv0.u = 0;
	uv0.v = 0;
	
	uv1.u = 0;
	uv1.v = 1;

	uv2.u = 1;
	uv2.v = 1;

	v0 = p_p0 - p_p1;
	v1 = p_p0 - p_p2;

	singleNormal = true;
	normal = v0.cross(v1);
	normal.normalize();
	
	initBoundingBox();
}

Triangle::Triangle(Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, UVMap p_uv0, UVMap p_uv1, UVMap p_uv2, Shader* p_shader) : Object3d(p_shader) {
	setName("triangle"); 
	p0 = p_p0;
	p1 = p_p1;
	p2 = p_p2;

	uv0 = p_uv0;
	uv1 = p_uv1;
	uv2 = p_uv2;

	v0 = p_p0 - p_p1;
	v1 = p_p0 - p_p2;

	singleNormal = true;
	normal = v0.cross(v1);
	normal.normalize();
	
	initBoundingBox();
}

Triangle::Triangle(Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, Vector3d p_normal0, Vector3d p_normal1, Vector3d p_normal2, Shader* p_shader) : Object3d(p_shader) {
	setName("triangle"); 
	p0 = p_p0;
	p1 = p_p1;
	p2 = p_p2;

	uv0.u = 0;
	uv0.v = 0;
	
	uv1.u = 0;
	uv1.v = 1;

	uv2.u = 1;
	uv2.v = 1;

	v0 = p_p0 - p_p1;
	v1 = p_p0 - p_p2;

	singleNormal = false;
	normal0 = p_normal0;
	normal1 = p_normal1;
	normal2 = p_normal2;

	normal = (normal0 + normal1 + normal2) / 3;
	
	initBoundingBox();
}

Triangle::Triangle(Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, Vector3d p_normal0, Vector3d p_normal1, Vector3d p_normal2, UVMap p_uv0, UVMap p_uv1, UVMap p_uv2, Shader* p_shader) : Object3d(p_shader) {
	setName("triangle"); 
	p0 = p_p0;
	p1 = p_p1;
	p2 = p_p2;

	uv0 = p_uv0;
	uv1 = p_uv1;
	uv2 = p_uv2;

	v0 = p_p0 - p_p1;
	v1 = p_p0 - p_p2;

	singleNormal = false;
	normal0 = p_normal0;
	normal1 = p_normal1;
	normal2 = p_normal2;

	normal = (normal0 + normal1 + normal2) / 3;

	initBoundingBox();
}

void Triangle::initBoundingBox() {
	float minX = min(p0.x, p1.x);
	minX = min(minX, p2.x);

	float minY = min(p0.y, p1.y);
	minY = min(minY, p2.y);

	float minZ = min(p0.z, p1.z);
	minZ = min(minZ, p2.z);

	float maxX = max(p0.x, p1.x);
	maxX = max(maxX, p2.x);

	float maxY = max(p0.y, p1.y);
	maxY = max(maxY, p2.y);

	float maxZ = max(p0.z, p1.z);
	maxZ = max(maxZ, p2.z);

	boundingBox.x0 = minX;
	boundingBox.x1 = maxX;
	boundingBox.y0 = minY;
	boundingBox.y1 = maxY;
	boundingBox.z0 = minZ;
	boundingBox.z1 = maxZ;
}

bool Triangle::isIntersectedBy(Ray* p_ray, float p_maxDist) {

	if(p_ray->getDir().dot(normal) > 0) {
		return false;
	}

	float tval;

	float A = v0.x;
	float B = v0.y;
	float C = v0.z;

	float D = v1.x;
	float E = v1.y;
	float F = v1.z;

	float G = p_ray->getDir().x;
	float H = p_ray->getDir().y;
	float I = p_ray->getDir().z;

	float J = p0.x - p_ray->getPoint().x;
	float K = p0.y - p_ray->getPoint().y;
	float L = p0.z - p_ray->getPoint().z;

	float EIHF = E * I - H * F;
	float GFDI = G * F - D * I;
	float DHEG = D * H - E * G;

	float denom = (A * EIHF + B * GFDI + C * DHEG);
	float beta = (J * EIHF + K * GFDI + L * DHEG) / denom;

	if(beta <= 0.0f || beta >= 1.0f) return false;

	float AKJB = A * K - J * B;
	float JCAL = J * C - A * L;
	float BLKC = B * L - K * C;

	float gamma = (I * AKJB + H * JCAL + G * BLKC) / denom;

	if(gamma <= 0.0f || beta + gamma >= 1.0f) return false;

	tval = -(F * AKJB + E * JCAL + D * BLKC) / denom;

	return (tval >= EPSILON && tval < p_maxDist);
}

bool Triangle::nearestIntersection(Ray* p_ray, RayIntersection* intersection) {

	if(p_ray->getDir().dot(normal) > 0) {
		return false;
	}

	float tval;

	float A = v0.x;
	float B = v0.y;
	float C = v0.z;

	float D = v1.x;
	float E = v1.y;
	float F = v1.z;

	float G = p_ray->getDir().x;
	float H = p_ray->getDir().y;
	float I = p_ray->getDir().z;

	float J = p0.x - p_ray->getPoint().x;
	float K = p0.y - p_ray->getPoint().y;
	float L = p0.z - p_ray->getPoint().z;

	float EIHF = E * I - H * F;
	float GFDI = G * F - D * I;
	float DHEG = D * H - E * G;

	float denom = (A * EIHF + B * GFDI + C * DHEG);
	float beta = (J * EIHF + K * GFDI + L * DHEG) / denom;

	if(beta <= 0.0f || beta >= 1.0f) return false;

	float AKJB = A * K - J * B;
	float JCAL = J * C - A * L;
	float BLKC = B * L - K * C;

	float gamma = (I * AKJB + H * JCAL + G * BLKC) / denom;

	if(gamma <= 0.0f || beta + gamma >= 1.0f) return false;

	tval = -(F * AKJB + E * JCAL + D * BLKC) / denom;

	if(tval >= EPSILON) {
		intersection->dist = tval;
		intersection->normal = normal;
		intersection->point = p_ray->getPointAt(tval);

		float alpha = 1.0f - beta -gamma;
		intersection->uv.u = alpha * uv0.u + beta * uv1.u + gamma * uv2.u;
		intersection->uv.v = alpha * uv0.v + beta * uv1.v + gamma * uv2.v;

		if(singleNormal) {
			intersection->normal = normal;
		} else {
			intersection->normal = normal0 * alpha + normal1 * beta + normal2 * gamma;
			intersection->normal.normalize();
		}

		intersection->object = this;

		return true;
	}

	return false;
}