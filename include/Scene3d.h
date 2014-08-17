#pragma once

#include "Camera.h"
#include "Object3d.h"
#include "Light.h"
#include "IBLLight.h"
#include "KDTree.h"
#include "Utils.h"
#include <stdio.h>

class Scene3d {
public:
	Scene3d(char * p_name) {name = p_name; background = NULL; kdTree = NULL;}

	char* getName() {return name;}

	//objects list functions
	void setObjects(Object3d** p_objects, int p_objectCount) {objects = p_objects; objectCount = p_objectCount;}
	Object3d** getObjects(void) {return objects;}
	void addObject(Object3d* p_obj, int p_pos) {if(p_pos < objectCount) { objects[p_pos] = p_obj;} }
	int getObjectCount(void) {return objectCount;}

	//objects list functions
	void setLights(Light** p_lights, int p_lightCount) {lights = p_lights; lightCount = p_lightCount;}
	Light** getLights(void) {return lights;}
	void addLight(Light* p_light, int p_pos) {if(p_pos < lightCount) { lights[p_pos] = p_light; } }
	int getLightCount(void) {return lightCount;}

	//IBL background 
	void setBackground(IBLLight *p_background) {background = p_background;}
	IBLLight* getBackground(void) {return background;}
	
	void initKDTree(int p_maxDepth, int p_minObjPerNode, bool p_verbose) {
		kdTree = new KDTree(objects, objectCount, p_maxDepth, p_minObjPerNode, p_verbose);
	}

	BoundingBox* getSceneBoundingBox() {
		Object3d* object;
		BoundingBox* totalBox = new BoundingBox(FAR_AWAY, -FAR_AWAY, FAR_AWAY, -FAR_AWAY, FAR_AWAY, -FAR_AWAY);
		BoundingBox* objBox;
		
		for(int i = 0; i < objectCount; i++) {
			object = objects[i];
			objBox = object->getBoundingBox();

			totalBox->adjustToInclude(objBox);
		}

		return totalBox;
	}

	char *name;
	Camera* camera;
	Object3d** objects;
	int objectCount; //number of elements in the objects array

	Light** lights;
	int lightCount; //number of elements in the lights array

	IBLLight* background;
	KDTree* kdTree;
};