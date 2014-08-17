#pragma once

#include "Vector3d.h"
#include "RGB.h"

class Light {
public:
	Light(char* p_name) {name = p_name;}

	void setName(char* p_name) {name = p_name;}
	char* getName() {return name;}

	virtual RGB getRadianceAt(Vector3d p_point) = 0;
	virtual Vector3d getDirectionAt(Vector3d p_point) = 0;
	virtual float getDistanceAt(Vector3d p_point) = 0;

	void newSampleAt(Vector3d p_point) {;}

protected:
	char *name;
};