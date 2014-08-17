#pragma once

#include "Utils.h" 
#include "Vector3d.h"
#include "UVMap.h"
#include "BoundingBox.h"
#include "Ray.h"

class Shader;
class Ray;

class Object3d {
public:
	Object3d(Shader* p_shader) {shader = p_shader; boundingBox= BoundingBox(0, 0, 0, 0, 0, 0);}

	void setName(char* p_name) {name = p_name;}
	char* getName() {return name;}

	void setShader(Shader* p_shader) {shader = p_shader;}
	Shader* getShader() {return shader;}

	virtual bool isIntersectedBy(Ray* p_ray, float p_maxDist) = 0;
	virtual bool nearestIntersection(Ray* p_ray, RayIntersection* intersection) = 0;

	virtual void initBoundingBox() = 0;
	BoundingBox* getBoundingBox() {return &boundingBox;}

protected:
	char *name;	
	Shader* shader;
	BoundingBox boundingBox;
};