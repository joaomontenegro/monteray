#include "PhongGlassShader.h"
#include "Object3d.h"

RGB PhongGlassShader::getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene) {
	return RGB(0,0,0);
}

int PhongGlassShader::getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values) {
	return 0;
}

int PhongGlassShader::getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVec, Ray* p_specularRays, RGB* p_values) {
	
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
	//refract - Snell
	float refIndexT = refrIndex; //transmitted
	float refIndexI = AIR_REFRACT_INDEX; //incident

	Vector3d inVec = p_inVec;
	float vecDotN = (inVec).dot(p_normal);
	float bb = (vecDotN >= 0)? refIndexT / refIndexI : refIndexI / refIndexT;
	float aa = 0;
	float D2 = 1 - bb * bb *(1 - vecDotN * vecDotN);
	
	if(D2 < 0) {
		return 0;
	}
	
	if(vecDotN > 0) {
		aa = bb * vecDotN - sqrt(D2);
	} else {
		aa = bb * vecDotN + sqrt(D2);
	}

	Vector3d refractedVec = inVec * bb - p_normal * aa;

	ONB onb;
	onb.initFromU(refractedVec);
	Vector3d outVec = onb.v * vec.x + onb.w * vec.y + onb.u * vec.z;
	p_specularRays[0].setPoint(p_point);
	p_specularRays[0].setDir(outVec);
	p_specularRays[0].setDiffuse(false);

	//Fresnel index:
	float cosT =  refractedVec.dot(p_normal);
	float cosI =  inVec.dot(p_normal);
	float indexTCosI = refIndexT * cosI;
	float indexICosT = refIndexI * cosT;
	float indexTCosT = refIndexT * cosT;
	float indexICosI = refIndexI * cosI;
	float rParal = (indexTCosI - indexICosT) / (indexTCosI + indexICosT); 
	float rPerp = (indexICosI - indexTCosT) / (indexICosI + indexTCosT);
	float r = (rParal * rParal + rPerp * rPerp) / 2;

	p_values[0] = color * (1-r);

	//reflect
	Vector3d reflectedVec = -((p_normal * dotProd2) - inVec);
	
	p_specularRays[1].setPoint(p_point);
	p_specularRays[1].setDir(reflectedVec);
	p_specularRays[1].setDiffuse(false);
	p_values[1] = color * (r);
	
	return 2;

	return 1;
}

bool PhongGlassShader::isLight() {
	return false;
}