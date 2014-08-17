#include "DistantLight.h"

DistantLight::DistantLight(char* p_name, Vector3d p_dir, RGB p_flux) : Light(p_name) {
	dir = p_dir; 
	dir.normalize();
	flux = p_flux;
}

RGB DistantLight::getRadianceAt(Vector3d p_point) {
	return flux;
}

Vector3d DistantLight::getDirectionAt(Vector3d p_point) {
	return dir;
}

float DistantLight::getDistanceAt(Vector3d p_point) {
	return FAR_AWAY;
}
