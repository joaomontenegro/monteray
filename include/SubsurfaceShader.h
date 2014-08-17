#pragma once

#include "Shader.h"
#include "ONB.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


class SubsurfaceShader: public Shader {
public:
	SubsurfaceShader() {color.randomize(); inColor.randomize(); n = 1;refrIndex = 1;};
	SubsurfaceShader(RGB p_color, RGB p_inColor, float p_n, float p_refrIndex) {color = p_color; inColor = p_inColor; n = p_n;refrIndex = 1;}

	RGB getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene);
	int getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values);
	int getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Ray* p_specularRays, RGB* p_values);

	RGB getDiffuseIrradiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene);
	Ray getSubsurfaceSampledRay(Object3d* p_object, Vector3d p_point, Vector3d p_normal, Vector3d p_inVector, UVMap p_uv);

	bool isLight();
	bool hasSubsurface();
private:
	RGB color;
	RGB inColor;
	float n;
	float refrIndex;
};
