#include "Raytracer.h"

Image* Raytracer::render() {
	Image* image = new Image(scene->camera->getWidth(), scene->camera->getHeight());

	srand((unsigned int)time(NULL));
	time_t startTime = time(NULL);
	int printGoal = (scene->camera->getWidth() * scene->camera->getHeight()) / 100;
	int count = 0;
	int percent = 0;
	time_t now = 0;

	printf("RENDERING with Raytracing:\n");
	for(int y = 0; y < scene->camera->getHeight(); y++) {
		for(int x = 0; x < scene->camera->getWidth(); x++) {

			if(count == printGoal) {
				count = 0;
				now = time(NULL);
				percent++;
				time_t secsToGo = (int)(now - startTime) * (100 - percent) / percent;
				printf("%d%%  to go:%d secs\n", percent, secsToGo);
			}
			count++;

			RGB rgb(0,0,0);
			for(int s = 0; s < samplesPerPixel; s++) {
				
				float jitterX = 0.0f;
				float jitterY = 0.0f;

				if(s > 0) {
					jitterX = (((float)(rand() % 1000)) / 1000.0f) - 0.5f;
					jitterY = (((float)(rand() % 1000)) / 1000.0f) - 0.5f;
				}

				Ray ray = scene->camera->castRay((float)x + jitterX, (float)y + jitterY);
				ray.setDiffuse(false);
				rgb = rgb + this->trace(ray, scene->camera->getPosition(), 0);
			}
			rgb = rgb / (float)samplesPerPixel;
			rgb.clamp();
			image->putPixel(x, y, rgb);
		}
	}
	printf("DONE... %d sec\n", (time(NULL) - startTime));

	return image;
}

RGB Raytracer::trace(Ray p_ray, Vector3d cameraPos, int p_depth) {
	RGB radiance(0,0,0);
	RGB localRadiance(0,0,0);
	RayIntersection rayInt;
	RayIntersection nearestRayInt;

	nearestRayInt.dist = -1;
	p_depth++;

	bool intersected = false;
	for(int o = 0; o < scene->getObjectCount(); o++) {
		Object3d* obj = scene->getObjects()[o];
		if(obj->nearestIntersection(&p_ray, &rayInt)) {
			intersected = true;
			if(nearestRayInt.dist < 0 || rayInt.dist < nearestRayInt.dist) {
				nearestRayInt = rayInt;
				//local ilumination:
				Vector3d viewVec = cameraPos - rayInt.point;
				viewVec.normalize();
				localRadiance = obj->getShader()->getDiffuseRadiance(obj, rayInt.point, rayInt.normal, rayInt.uv, viewVec, scene);
				radiance = localRadiance;

				if(p_depth < maxDepth) {
					Vector3d outVecs[10];
					RGB values[10];
					int outVecsCount = obj->getShader()->getSpecularRays(obj,rayInt.point, rayInt.normal, rayInt.uv, viewVec, p_ray.getDir(), outVecs, values);
					
					for(int s = 0; s < outVecsCount; s++) {
						Ray specularRay(rayInt.point, outVecs[s]);
						radiance = radiance + (trace(specularRay, cameraPos, p_depth) * values[s]);
					}
				}
			}
		} 
	}

	if(!intersected && scene->getBackground() != NULL && !p_ray.getDiffuse()) {
		radiance = scene->getBackground()->getRadianceAt(p_ray.getPoint());
	}

	return radiance;
}