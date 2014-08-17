#pragma once

#include "Camera.h"
#include "Vector3d.h"
#include "Ray.h"
#include "ONB.h"
#include <math.h>

class PinholeCamera: public Camera {
public:
	PinholeCamera(Vector3d p_position, Vector3d p_target, Vector3d p_up, float p_fov, int p_width, int p_height);

	Ray castRay(float x, float y);

private:
	float au;
    float av;
	ONB onb;
};