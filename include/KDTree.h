#pragma once

#include "Object3d.h"
#include "IntersectAccelerator.h"
#include "BoundingBox.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


int const KD_AXIS_NONE = 0;
int const KD_AXIS_X = 1;
int const KD_AXIS_Y = 2;
int const KD_AXIS_Z = 3;

float const KD_INFINITY = 1000000.0f;

struct KDTreeNode {
	int axis;
	float cut;
	bool leaf;
	BoundingBox* bbox;
	Object3d** objList;
	int objCount;

	KDTreeNode *child1;
	KDTreeNode *child2;
};

class KDTree : IntersectAcceletator {
public:
	KDTree(Object3d** p_objList, int p_objCount, int p_maxDepth, int p_minObjPerNode, bool p_verbose);
	~KDTree();

	bool intersects(Ray* p_ray, float p_maxDist);
	bool intersects(Ray* p_ray, float p_maxDist, KDTreeNode* p_node);

	bool nearestIntersection(Ray* p_ray, RayIntersection* p_intersection);
	bool nearestIntersection(Ray* p_ray, RayIntersection* p_intersection, KDTreeNode* p_node);

	KDTreeNode * constructNode(Object3d** p_objList, int p_objCount, BoundingBox* p_bbox, int p_parentAxis, int p_depth);

	int chooseAxis(Object3d** p_objList, int p_objCount, BoundingBox* p_bbox, int p_parentAxis);
	float chooseCut(Object3d** p_objList, int p_objCount, BoundingBox* p_bbox, int p_axis);

	BoundingBox* getTotalBoundingBox(Object3d** p_objList, int p_objCount);
	
	KDTreeNode* root;

	bool verbose;
	int maxDepth;
	int minObjPerNode;

	private:
	float max(float a, float b) {
		return (a < b)? b: a;
	}

	void quicksort(float* list, int left, int right);
	int partition(float* list, int left, int right, int pivotIndex);
};