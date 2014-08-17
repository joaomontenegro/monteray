#include "Ray.h"
#include "Object3d.h"

Vector3d Ray::getPointAt(float p_dist) {
	return point + direction * p_dist;
}

bool Ray::intersects(BoundingBox* p_box, float t0, float t1) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	if (getDir().x >= 0) {
		tmin = (p_box->x0 - getPoint().x) / getDir().x;
		tmax = (p_box->x1 - getPoint().x) / getDir().x;
	}	else {
		tmin = (p_box->x1 - getPoint().x) / getDir().x;
		tmax = (p_box->x0 - getPoint().x) / getDir().x;
	}	if (getDir().y >= 0) {
		tymin = (p_box->y0 - getPoint().y) / getDir().y;
		tymax = (p_box->y1 - getPoint().y) / getDir().y;
	} else {
		tymin = (p_box->y1 - getPoint().y) / getDir().y;
		tymax = (p_box->y0 - getPoint().y) / getDir().y;
	}
	
	if ( (tmin > tymax) || (tymin > tmax) ) {
		return false;
	}
	
	if (tymin > tmin) {
		tmin = tymin;
	}
	
	if (tymax < tmax) {
		tmax = tymax;
	}
	
	if (getDir().z >= 0) {
		tzmin = (p_box->z0 - getPoint().z) / getDir().z;
		tzmax = (p_box->z1 - getPoint().z) / getDir().z;
	}	else {
		tzmin = (p_box->z1 - getPoint().z) / getDir().z;
		tzmax = (p_box->z0 - getPoint().z) / getDir().z;
	}
	
	if ( (tmin > tzmax) || (tzmin > tmax) ) {
		return false;
	}
	
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	
	if (tzmax < tmax) {
		tmax = tzmax;
	}
		
	return ( (tmin < t1) && (tmax > t0) );
}

bool Ray::intersects(Object3d** p_objList, int p_objCount, float p_maxDist) {
	for(int o = 0; o < p_objCount; o++) {
		Object3d* obj = p_objList[o];
		if(obj->isIntersectedBy(this, p_maxDist)) {
			return true;
		}
	}

	return false;
}

bool Ray::nearestIntersection(Object3d** p_objList, int p_objCount, RayIntersection* intersection) {
	bool alreadyIntersected = false;

	RayIntersection rayInt;

	for(int o = 0; o < p_objCount; o++) {
		Object3d* obj = p_objList[o];
		
		bool intersects = obj->nearestIntersection(this, &rayInt);
		if(intersects && (!alreadyIntersected || rayInt.dist < intersection->dist)) {
			intersection->dist = rayInt.dist;
			intersection->normal = rayInt.normal;
			intersection->object = rayInt.object;
			intersection->point = rayInt.point;
			intersection->uv = rayInt.uv;

			alreadyIntersected = true;
		}
	}

	return alreadyIntersected;
}

bool Ray::nearestIntersectionWithObject(Object3d* p_obj, RayIntersection* intersection) {
	RayIntersection rayInt;

	if(p_obj->nearestIntersection(this, &rayInt)) {
		intersection->dist = rayInt.dist;
		intersection->normal = rayInt.normal;
		intersection->object = rayInt.object;
		intersection->point = rayInt.point;
		intersection->uv = rayInt.uv;
		return true;
	}

	return false;
}