#pragma once

#include "Shader.h"
#include "Vector3d.h"
#include <math.h>

class GlassShader: public Shader {
public:
	GlassShader() {transmition.randomize(); refrIndex = 1.0;};
	GlassShader(RGB p_transmition, float p_refrIndex) {transmition = p_transmition; refrIndex = p_refrIndex;}

	RGB getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene);
	int getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values);
	int getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Ray* p_specularRays, RGB* p_values);
	bool isLight();
private:
	RGB transmition;
	float refrIndex;
};
