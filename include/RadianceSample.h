#pragma once

#include "RGB.h"
#include "Vector3d.h"

struct RadianceSample {
	RGB rgb;
	Vector3d direction;
};