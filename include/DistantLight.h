#pragma once

#include "Utils.h"
#include "Light.h"
#include "Vector3d.h"

class DistantLight : public Light {
public:
	DistantLight(char* p_name, Vector3d p_dir, RGB p_flux);

	RGB getRadianceAt(Vector3d p_point);
	Vector3d getDirectionAt(Vector3d p_point);
	float getDistanceAt(Vector3d p_point);
	
private:
	Vector3d dir;
	RGB flux;
	RGB fluxDiv4PI;
};