#pragma once

#include "Shader.h"
#include "Image.h"
#include "ONB.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


class TexturedLambertianShader: public Shader {
public:
	Image* map;
	RGB color;

	TexturedLambertianShader() {map = NULL; color.setValues(0.8f, 0.8f, 0.8f);}
	TexturedLambertianShader(Image* p_image) {map = p_image; color.setValues(0.8f, 0.8f, 0.8f);}
	TexturedLambertianShader(RGB p_color) {map = NULL; color = p_color;};
	TexturedLambertianShader(Image* p_image, RGB p_color) {map = p_image; color = p_color;}

	~TexturedLambertianShader() {delete map;}

	RGB getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene);
	int getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values);
	int getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Ray* p_specularRays, RGB* p_values);
	bool isLight();
	RGB getDiffuseReflectance(UVMap p_uv);

};
