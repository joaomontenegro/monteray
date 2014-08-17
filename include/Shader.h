#pragma once

#include "Vector3d.h"
#include "Scene3d.h"
#include "RGB.h"
#include "Ray.h"
#include "Image.h"

const float AIR_REFRACT_INDEX = 1.003f;

class Object3d;

class Shader {
public:
	virtual RGB getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene) = 0;
	virtual int getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values) = 0;
	virtual int getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Ray* p_specularRays, RGB* p_values) = 0;
	virtual bool isLight() = 0;

	virtual bool hasSubsurface() {return false;}
	virtual Ray getSubsurfaceSampledRay(Object3d* p_object, Vector3d p_point, Vector3d p_normal, Vector3d p_inVector, UVMap p_uv) {Ray ray; return ray;}

	virtual RGB getDiffuseReflectance(UVMap p_uv) {
		return RGB(0, 0, 0);
	}
};