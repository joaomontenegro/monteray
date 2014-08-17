#include "IBLLight.h"

IBLLight::IBLLight(Image* p_image, float p_multiplier) : Light("ibl") {
	image = p_image;
	this->multiplier = p_multiplier;
	gamma = 1.0f / p_multiplier;
}

RGB IBLLight::getRadianceAt(Vector3d p_point) {

	Vector3d dir;

	dir.x = samplePoint.z - p_point.z;
	dir.y = samplePoint.x - p_point.x;
	dir.z = samplePoint.y - p_point.y;
	
	float theta = acosf((samplePoint.z - p_point.z) / FAR_AWAY);
	float phi = atan2f(samplePoint.y - p_point.y, dir.x);

	if(phi < 0) {
		phi += 2 * PI;
	}

	float u = phi / (2*PI);
	float v = (PI - theta) / PI;

	RGB c = image->getTexel(u, v);
	RGB rad;
	rad = c * multiplier;
	return rad;
}

Vector3d IBLLight::getDirectionAt(Vector3d p_point) {
	sampleDir = p_point - samplePoint;
	sampleDir.normalize();
	return sampleDir;
}

float IBLLight::getDistanceAt(Vector3d p_point) {
	sampleDistance = (p_point - samplePoint).length();
	return sampleDistance;
}

void IBLLight::nextSample() {

	const float PI = 3.14159265358979323846f;
	//Malley's method. find a sample in the unit disk (r,theta) = (sqrt(rand1), 2pi*rand2)

	float r = sqrt(((float)(rand() % 1000)) / 1000.0f);
	float theta = (((float)(rand() % 1000)) / 1000.0f) * 2 * PI;

	samplePoint.x = r * cos(theta);
	samplePoint.y = r * sin(theta);
	samplePoint.z = sqrtf(1 - samplePoint.x * samplePoint.x - samplePoint.y * samplePoint.y);
	
}
