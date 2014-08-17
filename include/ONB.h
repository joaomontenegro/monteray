#pragma once

#include "Vector3d.h"

const float ONB_EPSILON = 0.01f;

class ONB {
public:
	void initFromU(Vector3d p_u);
	void initFromWV(Vector3d p_w, Vector3d p_v);

	Vector3d transform(Vector3d a);
	Vector3d untransform(Vector3d a);

	Vector3d u;
    Vector3d v;
    Vector3d w;
};

inline void ONB::initFromU(Vector3d p_u) {
	Vector3d n(1.0f, 0.0f, 0.0f);
	Vector3d m(0.0f, 1.0f, 0.0f);
	
	u = p_u.unitVector();
	v = u.cross(n);
	if(v.length() < ONB_EPSILON) {
		v = u.cross(m);
	}

	w = u.cross(v);
}

inline void ONB::initFromWV(Vector3d p_w, Vector3d p_v) {
	w = p_w.unitVector();
	u = (p_v.cross(w)).unitVector();
	v = (w.cross(u)).unitVector();
}

inline Vector3d ONB::transform(Vector3d a) {
	float x = (a.x * u.x) + (a.y * v.x) + (a.z * w.x);
	float y = (a.x * u.y) + (a.y * v.y) + (a.z * w.y);
	float z = (a.x * u.z) + (a.y * v.z) + (a.z * w.z);
	a.setX(x);
	a.setY(y);
	a.setZ(z);
	return a;
}

inline Vector3d ONB::untransform(Vector3d a) {
	a.setX(a.dot(u));
	a.setY(a.dot(v));
	a.setZ(a.dot(w));
	return a;
}