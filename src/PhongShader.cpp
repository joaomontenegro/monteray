#include "PhongShader.h"
#include "Object3d.h"

RGB PhongShader::getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene) {
	return RGB(0,0,0);
}

int PhongShader::getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values) {
	return 0;
}

int PhongShader::getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVec, Ray* p_specularRays, RGB* p_values) {

	const float PI = 3.14159265358979323846f;
	
	float r1 = ((float)(rand() % 1000)) / 1000.0f;
	float r2 = ((float)(rand() % 1000)) / 1000.0f;

	float a = sqrtf(1 - powf(r1, 2.0f / (n + 1.0f)));
	float phi = 2 * PI * r2;
	
	float x = a * cos(phi);
	float y = a * sin(phi);
	float z = powf(r1, 1.0f / (n + 1.0f));

	Vector3d vec(x, y, z);
	vec.normalize();

	float dotProd2 = 2 * p_inVec.dot(p_normal);
	Vector3d reflexVec = -((p_normal * dotProd2) - p_inVec);

	ONB onb;
	onb.initFromU(reflexVec);
	Vector3d outVec = onb.v * vec.x + onb.w * vec.y + onb.u * vec.z;
	p_specularRays[0].setPoint(p_point);
	p_specularRays[0].setDir(outVec);
	p_specularRays[0].setDiffuse(false);
	
	p_values[0] = this->color;

	return 1;
}

bool PhongShader::isLight() {
	return false;
}
