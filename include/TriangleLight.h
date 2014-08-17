#pragma once

#include "Light.h"
#include "Vector3d.h"
#include "Triangle.h"
#include "LambertianLightShader.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


class TriangleLight : public Light {
public:
	Triangle triangle;

	TriangleLight(char* p_name, Vector3d p_p0, Vector3d p_p1, Vector3d p_p2, RGB p_flux, LambertianLightShader* p_shader);

	RGB getRadianceAt(Vector3d p_point);
	Vector3d getDirectionAt(Vector3d p_point);
	float getDistanceAt(Vector3d p_point);
	
	void nextSample();
	
private:
	RGB flux;
	RGB fluxPI;
	RGB radiance;
	float area;
	
	Vector3d samplePoint;
	Vector3d sampleDir;
	float sampleDistance;
};