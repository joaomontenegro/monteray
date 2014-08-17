#pragma once

#include "Shader.h"
#include "ONB.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


class PhongGlassShader: public Shader {
public:
	PhongGlassShader() {color.randomize(); n = 1;refrIndex = 1;};
	PhongGlassShader(RGB p_color, float p_n, float p_refrIndex) {color = p_color; n = p_n;refrIndex = 1;}

	RGB getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene);
	int getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values);
	int getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Ray* p_specularRays, RGB* p_values);
	bool isLight();
private:
	RGB color;
	float n;
	float refrIndex;
};
