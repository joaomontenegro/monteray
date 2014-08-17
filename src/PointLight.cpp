#include "PointLight.h"

PointLight::PointLight(char* p_name, Vector3d p_point, RGB p_flux) : Light(p_name) {
	point = p_point; 
	flux = p_flux;
	const float PI = 3.14159265358979323846f;
	float div4PI = 1.0f / (4 * PI);
	fluxDiv4PI = flux * div4PI;
}

RGB PointLight::getRadianceAt(Vector3d p_point) {
	RGB rad;

	rad = fluxDiv4PI / (1 + sampleDistance * sampleDistance);
	return rad;
}

Vector3d PointLight::getDirectionAt(Vector3d p_point) {
	sampleDir = p_point - point;
	sampleDir.normalize();
	return sampleDir;
}

float PointLight::getDistanceAt(Vector3d p_point) {
	sampleDistance = (p_point - point).length();
	return sampleDistance;
}

