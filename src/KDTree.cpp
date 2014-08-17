#include "KDTree.h"

KDTree::KDTree(Object3d** p_objList, int p_objCount, int p_maxDepth, int p_minObjPerNode, bool p_verbose) {
	maxDepth = p_maxDepth;
	minObjPerNode = p_minObjPerNode;
	verbose = p_verbose;

	if(verbose) {
		printf("Using KD-Tree: %d depth | %d min obj p/ node\n", maxDepth, minObjPerNode);
	}
	time_t startTime = time(NULL);

	BoundingBox* totalBox = getTotalBoundingBox(p_objList, p_objCount);

	root = constructNode(p_objList, p_objCount, totalBox, KD_AXIS_NONE, 0);

	if(verbose) {
		printf("  KD-Tree took %d sec\n", (time(NULL) - startTime));
	}
}

KDTree::~KDTree() {
	if(verbose) {
		printf("Deleting KD Tree...\n");
	}
}

KDTreeNode * KDTree::constructNode(Object3d** p_objList, int p_objCount, BoundingBox* p_bbox, int p_parentAxis, int p_depth) {

	int depth = p_depth + 1;

	if(depth > maxDepth) {
		return NULL;
	}

	KDTreeNode * node = (KDTreeNode *) malloc(sizeof(KDTreeNode));

	node->objList = p_objList;
	node->objCount = p_objCount;
	node->bbox = p_bbox;
	
	//this node's objects

	Object3d** objList;	
	int objsOnNodeCount = 0;
	
	if(p_depth > 0) {
		short* objsOnNode = (short *)malloc(sizeof(short) * p_objCount);
		for(int i = 0; i < p_objCount; i++) {
			if(p_objList[i]->getBoundingBox()->intersects(p_bbox)) {
				objsOnNode[i] = 1;
				objsOnNodeCount++;
			} else {
				objsOnNode[i] = 0;
			}
		}

		if(objsOnNodeCount == p_objCount) {
			return NULL;
		}

		objList = new Object3d*[objsOnNodeCount];
		
		int o = 0;
		for(int i = 0; i < p_objCount; i++) {
			if(objsOnNode[i] == 1) {
				objList[o++] = p_objList[i];
			}
		}

		delete(objsOnNode);
	} else {
		objsOnNodeCount = p_objCount;
		objList = p_objList;
	}

	node->objCount = objsOnNodeCount;
	node->objList = objList;

	//process childs
	if(objsOnNodeCount < minObjPerNode) {
		node->leaf = true;
		node->child1 = NULL;
		node->child2 = NULL;

		//printf("LEAF2: %d < %d\n", objsOnNodeCount, minObjPerNode);
	} else {

		//the cut
		node->axis = chooseAxis(objList, objsOnNodeCount, p_bbox, p_parentAxis);
		node->cut = chooseCut(objList, objsOnNodeCount, p_bbox, node->axis);
	
		//cutted bouding boxes
		BoundingBox* bbox1 = new BoundingBox(p_bbox->x0, p_bbox->x1, p_bbox->y0, p_bbox->y1, p_bbox->z0, p_bbox->z1);
		BoundingBox* bbox2 = new BoundingBox(p_bbox->x0, p_bbox->x1, p_bbox->y0, p_bbox->y1, p_bbox->z0, p_bbox->z1);

		if(node->axis == KD_AXIS_X) {
			bbox1->x1 = node->cut;
			bbox2->x0 = node->cut;
		} else if(node->axis == KD_AXIS_Y) {
			bbox1->y1 = node->cut;
			bbox2->y0 = node->cut;
		} else if(node->axis == KD_AXIS_Z) {
			bbox1->z1 = node->cut;
			bbox2->z0 = node->cut;
		}

		//childs
		node->leaf = false;
		node->child1 = constructNode(objList, objsOnNodeCount, bbox1, p_parentAxis, depth);

		if(node->child1 != NULL) {
			node->child2 = constructNode(objList, objsOnNodeCount, bbox2, p_parentAxis, depth);
		} else {
			node->child2 = NULL;
		}

		if(node->child1 == NULL || node->child2 == NULL) {
			node->leaf = true;
			node->child1 = NULL;
			node->child2 = NULL;	
		}
	}

	if(node->leaf && verbose) {
		printf("   -> %d\n", objsOnNodeCount);
	}

	return node;
}

int KDTree::chooseAxis(Object3d** p_objList, int p_objCount, BoundingBox* p_bbox, int p_parentAxis) {
/*
	//roundRobin
	if(p_parentAxis == KD_AXIS_X) {
		return KD_AXIS_Y;
	} else if(p_parentAxis == KD_AXIS_Y) {
		return KD_AXIS_Z;
	} else {
		return KD_AXIS_X;
	}
*/
	
	float xd = p_bbox->x1 - p_bbox->x0;
	float yd = p_bbox->y1 - p_bbox->y0;
	float zd = p_bbox->z1 - p_bbox->z0;

	int axis = KD_AXIS_X;

	if(yd > xd && yd >= zd) {
		axis = KD_AXIS_Y;
	} else if(zd > xd && zd > yd) {
		axis = KD_AXIS_Z;
	}

	return axis;
}

void KDTree::quicksort(float* list, int left, int right) {
	if(right > left) {
		int pivotIndex = left + (right - left) / 2;
		int newPivotIndex = partition(list, left, right, pivotIndex);
		quicksort(list, left, newPivotIndex - 1);
		quicksort(list, newPivotIndex + 1, right);
	}
}

int KDTree::partition(float* list, int left, int right, int pivotIndex) {
	float pivotValue = list[pivotIndex];
	float tmp = list[right];
	list[right] = pivotValue;
	list[pivotIndex] = tmp;
	int storeIndex = left;

	for(int i = left; i < right - 1; i++) {
		if(list[i] < pivotValue) {
			tmp = list[storeIndex];
			list[storeIndex] = list[i];
			list[i] = tmp;
			storeIndex++;
		}

		tmp = list[right];
		list[right] = list[storeIndex];
		list[storeIndex] = tmp;
	}

	return storeIndex;
}


float KDTree::chooseCut(Object3d** p_objList, int p_objCount, BoundingBox* p_bbox, int p_axis) {

	float* list = (float *)malloc(sizeof(float) * p_objCount);

	float min, max;

	if(p_axis == KD_AXIS_X) {
		for(int i = 0; i < p_objCount; i++) {
			list[i] = p_objList[i]->getBoundingBox()->x0;
		}
		min = p_bbox->x0;
		max = p_bbox->x1;
	}

	if(p_axis == KD_AXIS_Y) {
		for(int i = 0; i < p_objCount; i++) {
			list[i] = p_objList[i]->getBoundingBox()->y0;
		}
		
		min = p_bbox->y0;
		max = p_bbox->y1;
	}

	if(p_axis == KD_AXIS_Z) {
		for(int i = 0; i < p_objCount; i++) {
			list[i] = p_objList[i]->getBoundingBox()->z0;
		}

		min = p_bbox->z0;
		max = p_bbox->z1;
	}

	quicksort(list, 0, p_objCount - 1);

	float median = list[p_objCount/2];

	delete list;

	return median;
	
	/*
	BoundingBox* bbox = p_bbox;
	float cut = 0;
	
	if(p_axis == KD_AXIS_X) {
		float xd = bbox->x1 - bbox->x0;
		cut = bbox->x0 + xd * 0.5f;
	}

	if(p_axis == KD_AXIS_Y) {
		float yd = bbox->y1 - bbox->y0;
		cut = bbox->y0 + yd  * 0.5f;
	}

	if(p_axis == KD_AXIS_Z) {
		float zd = bbox->z1 - bbox->z0;
		cut = bbox->z0 + zd  * 0.5f;
	}

	return cut;
	*/
/*
	int minIndex;
	Object3d* minObj;
	float minValue;
	Object3d* obj;
	float value;
	Object3d* objTmp;

	int halfArrayIndex = p_objCount / 2;

	for(int i = 0; i < halfArrayIndex; i++) {
		minIndex = i;
		minObj = p_objList[i];

		if(p_axis == KD_AXIS_X) {
			minValue = minObj->getBoundingBox()->x0;
		} else if(p_axis == KD_AXIS_Y) {
			minValue = minObj->getBoundingBox()->y0;
		} else if(p_axis == KD_AXIS_Z) {
			minValue = minObj->getBoundingBox()->z0;
		}

		
		for(int j = i + 1; j < p_objCount; j++) {
			obj = p_objList[j];

			if(p_axis == KD_AXIS_X) {
				value = obj->getBoundingBox()->x0;
			} else if(p_axis == KD_AXIS_Y) {
				value = obj->getBoundingBox()->y0;
			} else if(p_axis == KD_AXIS_Z) {
				value = obj->getBoundingBox()->z0;
			}

			if(value <	minValue) {
				minIndex = j;
				minValue = value;
			}
		}

		if(minIndex != i) {
			objTmp = p_objList[minIndex];
			p_objList[minIndex] = p_objList[i];
			p_objList[i] = objTmp;
		}
	}
	
	return minValue;
	*/
}

bool KDTree::intersects(Ray* p_ray, float p_maxDist) {
	return intersects(p_ray, p_maxDist, root);
}

bool KDTree::intersects(Ray* p_ray, float p_maxDist, KDTreeNode* p_node) {
	if(p_node == NULL || !p_ray->intersects(p_node->bbox, 0, p_maxDist)) {
		return false;
	}

	if(p_node->leaf) {
		for(int i = 0; i < p_node->objCount; i++) {
			return p_ray->intersects(p_node->objList, p_node->objCount, p_maxDist);
		}
	}

	return (intersects(p_ray, p_maxDist, p_node->child1) || intersects(p_ray, p_maxDist, p_node->child2));
}

bool KDTree::nearestIntersection(Ray* p_ray, RayIntersection* p_intersection) {
	return nearestIntersection(p_ray, p_intersection, root);
}

bool KDTree::nearestIntersection(Ray* p_ray, RayIntersection* p_intersection, KDTreeNode* p_node) {
	if(p_node == NULL || !p_ray->intersects(p_node->bbox, 0, KD_INFINITY)) {
		return false;
	}

	if(p_node->leaf) {
		for(int i = 0; i < p_node->objCount; i++) {
			return p_ray->nearestIntersection(p_node->objList, p_node->objCount, p_intersection);
		}
	}

	RayIntersection intersection1;
	RayIntersection intersection2;
	RayIntersection intersection;
	
	bool intersects1 = nearestIntersection(p_ray, &intersection1, p_node->child1);
	bool intersects2 = nearestIntersection(p_ray, &intersection2, p_node->child2);

	if(!intersects1 && !intersects2) {
		return false;
	}

	if(!intersects1) {
		intersection = intersection2;
	} else if(!intersects2) {
		intersection = intersection1;
	} else if(intersection1.dist < intersection2.dist) {
		intersection = intersection1;
	} else {
		intersection = intersection2;
	}

	p_intersection->dist = intersection.dist;
	p_intersection->normal = intersection.normal;
	p_intersection->object = intersection.object;
	p_intersection->point = intersection.point;
	p_intersection->uv = intersection.uv;

	return true;
}

BoundingBox* KDTree::getTotalBoundingBox(Object3d** p_objList, int p_objCount) {
	Object3d* object;
	BoundingBox* totalBox = new BoundingBox(KD_INFINITY, -KD_INFINITY, KD_INFINITY, -KD_INFINITY, KD_INFINITY, -KD_INFINITY);
	BoundingBox* objBox;
	
	for(int i = 0; i < p_objCount; i++) {
		object = p_objList[i];
		objBox = object->getBoundingBox();

		totalBox->adjustToInclude(objBox);
	}

	return totalBox;
}
