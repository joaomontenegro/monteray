#include "IrradianceCache.h"

IrradianceCache::IrradianceCache(float p_a, Vector3d p_center, float p_sideLength, int p_maxDepth) {
	a = p_a;
	aInv = 1 / a;
	maxDepth = p_maxDepth;
	root = createOctreeNode(p_center, p_sideLength);
	nSamples = 0;
}

IrradianceCache::~IrradianceCache() {
}

void IrradianceCache::deleteOctreeNode(OctreeNode* p_node) {
	for(int i = 0; i < 8; i++) {
		if(p_node->childs[i] != NULL) {
			deleteOctreeNode(p_node->childs[i]);
		}
	}

	delete p_node;
}

OctreeNode* IrradianceCache::createOctreeNode(Vector3d p_center, float p_sideLength) {
	return createOctreeNode(NULL, 0, p_center, p_sideLength);
}

OctreeNode* IrradianceCache::createOctreeNode(OctreeNode* p_parent, int p_childIndex, Vector3d p_center, float p_sideLength) {
	OctreeNode* node = new OctreeNode;
	node->center = p_center;
	node->sideLength = p_sideLength;
	node->color.randomize();

	for(int i = 0; i < 8; ++i) {
		node->childs[i] = NULL;
	}
	
	if(p_parent != NULL && p_childIndex < 8 && p_childIndex >= 0) {
		p_parent->childs[p_childIndex] = node;
	}

	return node;
}

int IrradianceCache::getChildIndex(OctreeNode* p_node, Vector3d p_point) {
	if(p_point.x >= p_node->center.x) {
		if(p_point.y >= p_node->center.y) {
			if(p_point.z >= p_node->center.z) {
				return 0;
			} else { //point z < node z
				return 1;
			}
		} else { //point y < node y
			if(p_point.z >= p_node->center.z) {
				return 2;
			} else { //point z < node z
				return 3;
			}
		}
	} else { //point x < node x
		if(p_point.y >= p_node->center.y) {
			if(p_point.z >= p_node->center.z) {
				return 4;
			} else { //point z < node z
				return 5;
			}
		} else { //point y < node y
			if(p_point.z >= p_node->center.z) {
				return 6;
			} else { //point z < node z
				return 7;
			}
		}
	}
}

Vector3d IrradianceCache::calculateChildCenter(OctreeNode* p_node, int p_childIndex) {
	Vector3d childCenter = p_node->center;
	float delta = p_node->sideLength / 4;

	if(p_childIndex == 0) {
		childCenter.x += delta;
		childCenter.y += delta;
		childCenter.z += delta;
	} else if(p_childIndex == 1) {
		childCenter.x += delta;
		childCenter.y += delta;
		childCenter.z -= delta;
	} else if(p_childIndex == 2) {
		childCenter.x += delta;
		childCenter.y -= delta;
		childCenter.z += delta;
	} else if(p_childIndex == 3) {
		childCenter.x += delta;
		childCenter.y -= delta;
		childCenter.z -= delta;
	} else if(p_childIndex == 4) {
		childCenter.x -= delta;
		childCenter.y += delta;
		childCenter.z += delta;
	} else if(p_childIndex == 5) {
		childCenter.x -= delta;
		childCenter.y += delta;
		childCenter.z -= delta;
	} else if(p_childIndex == 6) {
		childCenter.x -= delta;
		childCenter.y -= delta;
		childCenter.z += delta;
	} else if(p_childIndex == 7) {
		childCenter.x -= delta;
		childCenter.y -= delta;
		childCenter.z -= delta;
	}

	return childCenter;
}

OctreeNode* IrradianceCache::addIrradianceSample(IrradianceSample p_irradiance) {
	float domain = a * p_irradiance.radius;
	float domain2 = 2 * domain;

	OctreeNode* node = root;
	int childIndex;
	Vector3d center;
	int i = 0;
	
	for(i = 0; i < maxDepth; ++i) {
		if(node->sideLength < domain2) {
			break;
		}
		childIndex = getChildIndex(node, p_irradiance.point);
		if(node->childs[childIndex] == NULL) {
			center = calculateChildCenter(node, childIndex);
			createOctreeNode(node, childIndex, center, node->sideLength / 2);
		}
		node = node->childs[childIndex];
	}
	
	node->irradiances.push_back(p_irradiance);

	nSamples++; 
	return node;
}

int IrradianceCache::searchIrradiances(Vector3d p_point, Vector3d p_normal, vector<IrradianceSample>* p_irradiances) {
	searchIrradiances(root, p_point, p_normal, p_irradiances);

	return (int)p_irradiances->size();
}

void IrradianceCache::searchIrradiances(OctreeNode* p_node, Vector3d p_point, Vector3d p_normal, vector<IrradianceSample>* p_irradiances) {
	vector<IrradianceSample>::iterator it;
	IrradianceSample irradiance;
	float weight;

	for(it = p_node->irradiances.begin(); it < p_node->irradiances.end(); ++it) {
		irradiance = *it;
		if(irradiance.point == p_point) {
			irradiance.weight = INFINITY;
			p_irradiances->push_back(irradiance);
			continue;
		}

		if(calculateDiscard(irradiance, p_point, p_normal) > -EPSILON) {
			weight = calculateWeight(irradiance, p_point, p_normal);
			
			if(weight > aInv) {
				irradiance.weight = weight;
				p_irradiances->push_back(irradiance);
			}
		}
	}
	
	float dx, dy, dz;
	for(int i = 0; i < 8; ++i) {
		if(p_node->childs[i] != NULL) {
			dx = fabs(p_node->childs[i]->center.x - p_point.x);
			dy = fabs(p_node->childs[i]->center.x - p_point.x);
			dz = fabs(p_node->childs[i]->center.x - p_point.x);
			if(dx < p_node->sideLength / 2 && dy < p_node->sideLength / 2 && dz < p_node->sideLength / 2) {
				searchIrradiances(p_node->childs[i], p_point, p_normal, p_irradiances);
			}
		}
	}

}

float IrradianceCache::calculateWeight(IrradianceSample p_irradiance, Vector3d p_point, Vector3d p_normal) {
	//to avoid division by zero in the translational difference
	if(p_irradiance.radius < SMALL_EPSILON) {
		return 0;
	} 

	//translational difference
	float transDiff;
	transDiff = (p_irradiance.point - p_point).length();
	if(p_irradiance.radius < INFINITY) {
		transDiff /= p_irradiance.radius;
	}

	//rotational difference
	float rotDiff = 1 - (p_irradiance.normal.dot(p_normal)); 
	rotDiff = (rotDiff < 0)? 0: rotDiff;
	rotDiff = sqrt(rotDiff);

	float difference = transDiff + rotDiff;

	//to avoid division by zero
	if(difference < SMALL_EPSILON) {
		return INFINITY;
	}

	float weight = 1 / (transDiff + rotDiff);

	return weight;
}
	
float IrradianceCache::calculateDiscard(IrradianceSample p_irradiance, Vector3d p_point, Vector3d p_normal) {
	Vector3d diff = p_point - p_irradiance.point;
	Vector3d normalAvg = (p_normal + p_irradiance.normal) / 2;
	return diff.dot(normalAvg);
}

