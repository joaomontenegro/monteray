#include "SubsurfaceShader.h"
#include "Object3d.h"

RGB SubsurfaceShader::getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene) {
	RGB totalRadiance(0,0,0);
	Light* light;

	for(int l = 0; l < p_scene->getLightCount(); l++) {
		light = p_scene->getLights()[l];
		Vector3d pointInLight = light->getNextPoint();
		Vector3d normalInLight = light->getNormalAt(pointInLight);
		Vector3d shadowRayVec = pointInLight - p_point;
		Vector3d shadowRayVecNorm = shadowRayVec.unitVector();

		float cosThetaLight = (light->getArea() == 0)? 1: -shadowRayVecNorm.dot(normalInLight);

		if(cosThetaLight > 0.0) {
			float cosThetaSurface = shadowRayVecNorm.dot(p_normal);

			if(cosThetaSurface > 0.0) {
				//shadow ray
				Ray shadowRay(p_point, shadowRayVecNorm);

				float distToLight = shadowRayVec.length();
				float distToLight2 = distToLight * distToLight;
				bool intersects = false;
				if(p_scene->kdTree == NULL) {
					intersects = shadowRay.intersects(p_scene->getObjects(), p_scene->getObjectCount(), distToLight);
				} else {
					intersects = p_scene->kdTree->intersects(&shadowRay, distToLight);
				}

				if(!intersects) {
					float g = cosThetaLight * cosThetaSurface / distToLight2;
					//float g = 1;
					RGB radiance = light->getRadianceAt(p_point, pointInLight);

					if(light->getArea() > 0.0f) {
						radiance = radiance * light->getArea(); // pdf = 1/A
					}

					totalRadiance = totalRadiance + (radiance * g);

				}
			}
		}
	}

	return (totalRadiance * color);
}

int SubsurfaceShader::getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values) {
	return 0;
}

int SubsurfaceShader::getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVec, Ray* p_specularRays, RGB* p_values) {
	
	const float PI = 3.14159265358979323846f;
	//Malley's method. find a sample in the unit disk (r,theta) = (sqrt(rand1), 2pi*rand2)

	float r = sqrtf(((float)(rand() % 1000)) / 1000.0f);
	float theta = (((float)(rand() % 1000)) / 1000.0f) * 2 * PI;

	float x = r * cos(theta);
	float y = r * sin(theta);
	float z = sqrtf(1 - x*x - y*y);

	ONB onb;
	onb.initFromU(p_normal);
	Vector3d outVec = onb.v * x + onb.w * y + onb.u * z;
	p_specularRays[0].setPoint(p_point);
	p_specularRays[0].setDir(outVec);
	p_specularRays[0].setDiffuse(true);
	
	p_values[0] = this->color;

	return 1;
}

bool SubsurfaceShader::isLight() {
	return false;
}

bool SubsurfaceShader::hasSubsurface() {
	return true;
}

RGB SubsurfaceShader::getDiffuseIrradiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene) {
	RGB totalRadiance(0,0,0);
	Light* light;

	for(int l = 0; l < p_scene->getLightCount(); l++) {
		light = p_scene->getLights()[l];
		Vector3d pointInLight = light->getNextPoint();
		Vector3d normalInLight = light->getNormalAt(pointInLight);
		Vector3d shadowRayVec = pointInLight - p_point;
		Vector3d shadowRayVecNorm = shadowRayVec.unitVector();

		float cosThetaLight = (light->getArea() == 0)? 1: -shadowRayVecNorm.dot(normalInLight);

		if(cosThetaLight > 0.0) {
			float cosThetaSurface = shadowRayVecNorm.dot(p_normal);

			if(cosThetaSurface > 0.0) {
				//shadow ray
				Ray shadowRay(p_point, shadowRayVecNorm);

				float distToLight = shadowRayVec.length();
				float distToLight2 = distToLight * distToLight;
				bool intersects = false;
				if(p_scene->kdTree == NULL) {
					intersects = shadowRay.intersects(p_scene->getObjects(), p_scene->getObjectCount(), distToLight);
				} else {
					intersects = p_scene->kdTree->intersects(&shadowRay, distToLight);
				}

				if(!intersects) {
					float g = cosThetaLight * cosThetaSurface / distToLight2;
					//float g = 1;
					RGB radiance = light->getRadianceAt(p_point, pointInLight);

					if(light->getArea() > 0.0f) {
						radiance = radiance * light->getArea(); // pdf = 1/A
					}

					totalRadiance = totalRadiance + (radiance * g);
				}
			}
		}
	}

	return (totalRadiance * inColor);

	//return inColor;
}

Ray SubsurfaceShader::getSubsurfaceSampledRay(Object3d* p_object, Vector3d p_point, Vector3d p_normal, Vector3d p_inVector, UVMap p_uv) {
	/*const float PI = 3.14159265358979323846f;
	
	float r1 = ((float)(rand() % 1000)) / 1000.0f;
	float r2 = ((float)(rand() % 1000)) / 1000.0f;

	float a = sqrtf(1 - powf(r1, 2.0f / (n + 1.0f)));
	float phi = 2 * PI * r2;
	
	float x = a * cos(phi);
	float y = a * sin(phi);
	float z = powf(r1, 1.0f / (n + 1.0f));

	Vector3d vec(x, y, z);
	vec.normalize();

	ONB onb;
	onb.initFromU(-p_normal);
	Vector3d outVec = onb.v * vec.x + onb.w * vec.y + onb.u * vec.z;
	*/

	const float PI = 3.14159265358979323846f;
	float r = sqrtf(((float)(rand() % 1000)) / 1000.0f);
	float theta = (((float)(rand() % 1000)) / 1000.0f) * 2 * PI;
	float x = r * cos(theta);
	float y = r * sin(theta);
	float z = sqrtf(1 - x*x - y*y);
	ONB onb;
	onb.initFromU(-p_normal);
	Vector3d outVec = onb.v * x + onb.w * y + onb.u * z;

	Ray ray(p_point, outVec);

	return ray;
}