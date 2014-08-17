#include "Utils.h"
#include "Vector3d.h"
#include "RGB.h"

#include <iostream>
#include <vector>

using namespace std;

typedef struct IrradianceSample {
	Vector3d point;
	Vector3d normal;
	Vector3d transGradient;
	Vector3d rotGradient;
	RGB irradiance;
	float radius;
	float weight;
} IrradianceSample;

typedef struct HemisphereSamples {
	
} HemisphereSamples;

typedef struct OctreeNode {
	Vector3d center;
	float sideLength;
	vector<IrradianceSample> irradiances;
	RGB color;
	
	OctreeNode* childs[8];

} OctreeNode;

class IrradianceCache {
public:
	OctreeNode* root;
	int nSamples;

	IrradianceCache(float p_a, Vector3d p_center, float p_sideLength, int p_maxDepth);

	~IrradianceCache();

	OctreeNode* addIrradianceSample(IrradianceSample p_irradiance);
	int searchIrradiances(Vector3d p_point, Vector3d p_normal, vector<IrradianceSample>* p_irradiances);
	float calculateWeight(IrradianceSample p_irradiance, Vector3d p_point, Vector3d p_normal);
	float calculateDiscard(IrradianceSample p_irradiance, Vector3d p_point, Vector3d p_normal);

private:
	int maxDepth;
	float a;
	float aInv;

	OctreeNode* createOctreeNode(Vector3d p_center, float p_sideLength);
	OctreeNode* createOctreeNode(OctreeNode* p_parent, int p_childIndex, Vector3d p_center, float p_sideLength);
	int getChildIndex(OctreeNode* p_node, Vector3d p_point);
	Vector3d calculateChildCenter(OctreeNode* p_node, int p_childIndex);
	void searchIrradiances(OctreeNode* p_node, Vector3d p_point, Vector3d p_normal, vector<IrradianceSample>* p_irradiances);
	void deleteOctreeNode(OctreeNode* p_node);
};
