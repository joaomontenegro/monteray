#include "MirrorShader.h"
#include "Object3d.h"

RGB MirrorShader::getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene) {
	return RGB(0,0,0);
}

int MirrorShader::getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVec, Vector3d* p_specularRays, RGB* p_values) {
	float dotProd2 = 2 * p_inVec.dot(p_normal);
	Vector3d outVec = -((p_normal * dotProd2) - p_inVec);

	p_specularRays[0] = outVec;

	p_values[0] = this->color;

	return 1;
}

int MirrorShader::getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVec, Ray* p_specularRays, RGB* p_values) {
	float dotProd2 = 2 * p_inVec.dot(p_normal);
	Vector3d outVec = -((p_normal * dotProd2) - p_inVec);

	p_specularRays[0].setPoint(p_point);
	p_specularRays[0].setDir(outVec);

	p_values[0] = this->color;

	return 1;
}

bool MirrorShader::isLight() {
	return false;
}