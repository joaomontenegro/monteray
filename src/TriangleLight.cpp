#pragma once

#include "TriangleLight.h"

TriangleLight::TriangleLight(char* p_name, Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, RGB p_flux, LambertianLightShader* p_shader) : Light(p_name) {
	const float PI = 3.14159265358979323846f;
	triangle = Triangle(p_p0, p_p1, p_p2, NULL);

	flux = p_flux;
	fluxPI = p_flux / PI;
	float div4PI = 1.0f / (4 * PI);

	area = triangle.v0.cross(triangle.v1).length() * 0.5f;
	radiance = fluxPI / area;

	p_shader->radiance = radiance;
	triangle.setShader(p_shader);
}

RGB TriangleLight::getRadianceAt(Vector3d p_point) {
	RGB rad;

	float len2 = sampleDistance * sampleDistance;
	float dot = sampleDir.dot(triangle.normal);
	rad = radiance * dot * area / len2;
	return rad;
}


Vector3d TriangleLight::getDirectionAt(Vector3d p_point) {
	sampleDir = p_point - samplePoint;
	sampleDir.normalize();
	return sampleDir;
}

float TriangleLight::getDistanceAt(Vector3d p_point) {
	sampleDistance = (p_point - samplePoint).length();
	return sampleDistance;
}

void TriangleLight::nextSample() {
	float rand1 = (((float)(rand() % 1000)) / 1000.0f);
	float rand2 = (((float)(rand() % 1000)) / 1000.0f);

	float s = sqrt(1.0f - rand1);
    float u = rand2 * s;
    float v = 1.0f - s;
    
	Vector3d v1 = (triangle.p1 - triangle.p0) * u;
    Vector3d v2 = (triangle.p2 - triangle.p0) * v;

	samplePoint = (triangle.p0 + v1 + v2 + triangle.normal * 0.01f);
}
