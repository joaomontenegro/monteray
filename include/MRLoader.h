#pragma once
#include "Scene3d.h"
#include "Camera.h"
#include "PinholeCamera.h"
#include "Light.h"
#include "PointLight.h"
#include "TriangleLight.h"
#include "IBLLight.h"
#include "Shader.h"
#include "LambertianShader.h"
#include "LambertianLightShader.h"
#include "TexturedLambertianShader.h"
#include "Image.h"
#include "MirrorShader.h"
#include "GlassShader.h"
#include "PhongGlassShader.h"
#include "PhongShader.h"
#include "Object3d.h"
#include "Triangle.h"
#include "KDTree.h"
#include "Sphere.h"

#include "Raytracer.h"
#include "Pathtracer.h"

#include <iostream>
#include <fstream>


class MRLoader {
public:
	Renderer* loadMRFile(char* p_filename);

	Renderer* loadRenderer(Scene3d* p_scene, ifstream* p_inFile);
	bool loadKDTree(Scene3d* p_scene, ifstream* p_inFile, int* p_maxDepth, int* p_minObjPerNode, bool* p_verbose);
	bool loadCamera(Scene3d* p_scene, ifstream* p_inFile);
	bool loadLight(Scene3d* p_scene, ifstream* p_inFile);
	bool loadShader(Shader** p_shaders, ifstream* p_inFile);
	bool loadObject(Scene3d* p_scene, Shader** p_shaders, int p_shaderCount, ifstream* p_inFile);
};