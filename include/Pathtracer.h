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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

class Pathtracer: public Renderer {
public:
	Pathtracer(Scene3d* p_scene, int p_samplesPerPixel, int p_maxDepth) {scene = p_scene; samplesPerPixel = p_samplesPerPixel; maxDepth = p_maxDepth;}
	Image* render();

private:
	RGB trace(Ray p_ray, int p_depth);

	int samplesPerPixel;
	int maxDepth;
};