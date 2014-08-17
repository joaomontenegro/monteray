#pragma once
#include "Scene3d.h"
#include "Camera.h"
#include "PinholeCamera.h"
#include "Light.h"
#include "PointLight.h"
#include "DistantLight.h"
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
#include "SubsurfaceShader.h"
#include "Object3d.h"
#include "Triangle.h"
#include "KDTree.h"
#include "Sphere.h"

#include "Raytracer.h"
#include "Pathtracer.h"
#include "ICPathtracer.h"
#include "SubsurfacePathtracer.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

typedef struct {
	int v0;
	int v1;
	int v2;

	int vn0;
	int vn1;
	int vn2;

	int vt0;
	int vt1;
	int vt2;
	int mtlId; //material id
} OBJ_f;

typedef struct {
	float kdr;float kdg;float kdb;
	float kar;float kag;float kab;
	float ksr;float ksg;float ksb;
	float Tr; // same as 'd'
	float Ns;
	string map;
} OBJ_mtl;

class OBJLoader {
public:
	OBJLoader() {mtlId = -1; mtlFilename = ""; currentMtlName = ""; objDir = ".";}

	Renderer* loadOBJFile(char* p_filename, char* p_MRSFilename);

private:
	bool parseOBJLine(string* lineBuffer, int lineNumber);
	bool parseMTLLine(string* lineBuffer, int lineNumber);
	int getMtlId(string mtlName);
	Renderer* createRenderer(char* p_MRSFilename);

	vector<Vector3d> vertices;
	vector<Vector3d> normals;
	vector<UVMap> uvs;
	vector<OBJ_f> faces;
	vector<string> mtlNames;
	vector<Shader*> shaders;

	int mtlId;
	string mtlFilename;
	string currentMtlName;
	string objDir;
};