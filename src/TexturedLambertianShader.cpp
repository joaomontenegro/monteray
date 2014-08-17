#include "TexturedLambertianShader.h"
#include "Object3d.h"

RGB TexturedLambertianShader::getDiffuseRadiance(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Scene3d* p_scene) {
	RGB totalRadiance(0,0,0);
	Light* light;

	for(int l = 0; l < p_scene->getLightCount(); l++) {
		light = p_scene->getLights()[l];

		Vector3d lightDirection = -light->getDirectionAt(p_point);
		float cosThetaSurface = lightDirection.dot(p_normal);

			if(cosThetaSurface > -EPSILON) {
				//shadow ray

				Ray shadowRay(p_point, lightDirection);

				float distToLight = light->getDistanceAt(p_point);
				bool intersects = false;
				if(p_scene->kdTree == NULL) {
					intersects = shadowRay.intersects(p_scene->getObjects(), p_scene->getObjectCount(), distToLight);
				} else {
					intersects = p_scene->kdTree->intersects(&shadowRay, distToLight);
				}

				if(!intersects) {
					totalRadiance = totalRadiance + light->getRadianceAt(p_point) * cosThetaSurface;
				}
			}
	}

	RGB c = (map != NULL)? map->getTexel(p_uv.u, p_uv.v) : color;
	return (totalRadiance * c);
}

int TexturedLambertianShader::getSpecularRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVector, Vector3d* p_specularRays, RGB* p_values) {
	return 0;
}

int TexturedLambertianShader::getBRDFSampledRays(Object3d* p_object, Vector3d p_point, Vector3d p_normal, UVMap p_uv, Vector3d p_viewVec, Vector3d p_inVec, Ray* p_specularRays, RGB* p_values) {

	const float PI = 3.14159265358979323846f;
	//Malley's method. find a sample in the unit disk (r,theta) = (sqrt(rand1), 2pi*rand2)

	float r = sqrt(((float)(rand() % 1000)) / 1000.0f);
	float theta = (((float)(rand() % 1000)) / 1000.0f) * 2 * PI;

	float x = r * cos(theta);
	float y = r * sin(theta);
	float z = sqrt(1 - x*x - y*y);

	ONB onb;
	onb.initFromU(p_normal);
	Vector3d outVec = onb.v * x + onb.w * y + onb.u * z;
	p_specularRays[0].setPoint(p_point);
	p_specularRays[0].setDir(outVec);
	p_specularRays[0].setDiffuse(true);
	
	RGB c = (map != NULL)? map->getTexel(p_uv.u, p_uv.v) : color;
	p_values[0] = c;

	return 1;
}

bool TexturedLambertianShader::isLight() {
	return false;
}

RGB TexturedLambertianShader::getDiffuseReflectance(UVMap p_uv) {
	RGB c = (map != NULL)? map->getTexel(p_uv.u, p_uv.v) : color;
	return c;
}