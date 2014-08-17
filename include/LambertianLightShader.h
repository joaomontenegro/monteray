#pragma once

#include "Shader.h"
#include "ONB.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


class LambertianLightShader: public Shader {
public:
	LambertianLightShader() {}
	LambertianLightShader(RGB p_radiance) {radiance = p_radiance;}

	RGB getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene);
	int getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values);
	int getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Ray* p_specularRays, RGB* p_values);
	bool isLight();

	RGB radiance;
};
