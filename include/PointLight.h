#pragma once

#include "Light.h"
#include "Vector3d.h"

class PointLight : public Light {
public:
	PointLight(char* p_name, Vector3d p_point, RGB p_flux);

	RGB getRadianceAt(Vector3d p_point);
	Vector3d getDirectionAt(Vector3d p_point);
	float getDistanceAt(Vector3d p_point);

private:
	Vector3d point;
	RGB flux;
	RGB fluxDiv4PI;
	Vector3d sampleDir;
	float sampleDistance;

};