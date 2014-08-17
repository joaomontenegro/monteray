#pragma once

#include "Ray.h"

class IntersectAcceletator {
public:
	virtual bool intersects(Ray* p_ray, float p_maxDist) = 0;
	virtual bool nearestIntersection(Ray* p_ray, RayIntersection* intersection) = 0;
};