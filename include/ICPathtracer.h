#pragma once

#include "Renderer.h"
#include "RGB.h"
#include "Light.h"
#include "Object3d.h"
#include "Shader.h"
#include "Image.h"
#include "Scene3d.h"
#include "Camera.h"
#include "Ray.h"
#include "KDTree.h"
#include "IrradianceCache.h"
#include "BoundingBox.h"
#include "ONB.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

const float M_PI = 3.14159265358979323846f;
const float IC_A = 0.3f;
const int IC_DEPTH = 100;
const int IC_SAMPLES = 128;

class ICPathtracer: public Renderer {
public:
	ICPathtracer(Scene3d* p_scene, int p_samplesPerPixel, int p_maxDepth) {scene = p_scene; samplesPerPixel = p_samplesPerPixel; maxDepth = p_maxDepth;}
	Image* render();

private:
	void calculateIrradiance(Vector3d p_point, Vector3d p_normal, IrradianceSample* irradiance);
	RGB trace(Ray p_ray, RayIntersection* p_intersection, int p_depth);
	

	int samplesPerPixel;
	int maxDepth;
};