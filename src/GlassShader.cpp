#include "GlassShader.h"
#include "Object3d.h"

RGB GlassShader::getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene) {
	return RGB(0,0,0);
}

int GlassShader::getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVec, Vector3d* p_specularRays, RGB* p_values) {
	//refract - Snell
	float refIndexT = refrIndex; //transmitted
	float refIndexI = AIR_REFRACT_INDEX; //incident

	Vector3d inVec = p_inVec;
	float vecDotN = (inVec).dot(p_normal);
	float b = (vecDotN >= 0)? refIndexT / refIndexI : refIndexI / refIndexT;
	float a = 0;
	float D2 = 1 - b * b *(1 - vecDotN * vecDotN);
	
	if(D2 < 0) {
		return 0;
	}
	
	if(vecDotN > 0) {
		a = b * vecDotN - sqrt(D2);
	} else {
		a = b * vecDotN + sqrt(D2);
	}

	Vector3d refractedVec = inVec * b - p_normal * a;

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

	p_specularRays[0] = refractedVec;
	p_values[0] = transmition * (1-r);

	//reflect
	float dotProd2 = vecDotN * 2;
	Vector3d reflectedVec = -((p_normal * dotProd2) - inVec);
	
	p_specularRays[1] = reflectedVec;
	p_values[1] = transmition * (r);
	
	return 2;
}

int GlassShader::getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVec, Ray* p_specularRays, RGB* p_values) {
	//refract - Snell
	float refIndexT = refrIndex; //transmitted
	float refIndexI = AIR_REFRACT_INDEX; //incident

	Vector3d inVec = p_inVec;
	float vecDotN = inVec.dot(p_normal);
	float b = (vecDotN >= 0)? refIndexT / refIndexI : refIndexI / refIndexT;
	float a = 0;
	float D2 = 1 - b * b *(1 - vecDotN * vecDotN);

	if(D2 < 0) {
		return 0;
	}
	
	if(vecDotN > 0) {
		a = b * vecDotN - sqrt(D2);
	} else {
		a = b * vecDotN + sqrt(D2);
	}

	Vector3d refractedVec = inVec * b - p_normal * a;

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

	p_specularRays[0].setPoint(p_point);
	p_specularRays[0].setDir(refractedVec);
	p_values[0] = transmition * (1-r);

	//reflect
	float dotProd2 = vecDotN * 2;
	Vector3d reflectedVec = -((p_normal * dotProd2) - inVec);
	
	p_specularRays[1].setPoint(p_point);
	p_specularRays[1].setDir(reflectedVec);

	p_values[1] = transmition * (r);


	if(!(p_values[1].g > -2 && p_values[1].g < 2)) {
		printf("%f %f \n", vecDotN, b);
	}
	
	return 2;
}

bool GlassShader::isLight() {
	return false;
}