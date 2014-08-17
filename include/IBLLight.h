#pragma once

#include "Utils.h"
#include "Light.h"
#include "Vector3d.h"
#include "Image.h"


#include <stdlib.h>
#include <stdio.h>
#include <time.h>


class IBLLight : public Light {
public:
	IBLLight(Image* p_image, float p_multiplier);

	RGB getRadianceAt(Vector3d p_point);
	Vector3d getDirectionAt(Vector3d p_point);
	float getDistanceAt(Vector3d p_point);
	
	void nextSample();

private:
	Image* image;
	float multiplier;
	float gamma;

	Vector3d samplePoint;
	Vector3d sampleDir;
	float sampleDistance;
};