#include "Pathtracer.h"

Image* Pathtracer::render() {
	Image* image = new Image(scene->camera->getWidth(), scene->camera->getHeight());

	srand((unsigned int)time(NULL));
	time_t startTime = time(NULL);
	int printGoal = (scene->camera->getWidth() * scene->camera->getHeight()) / 100;
	int count = 0;
	int percent = 0;
	time_t now = 0;
	printf("RENDERING with Pathtracing:\n");
	for(int y = 0; y < scene->camera->getHeight(); y++) {
		for(int x = 0; x < scene->camera->getWidth(); x++) {
			
			if(count == printGoal) {
				count = 0;
				now = time(NULL);
				percent++;
				time_t secsToGo = (int)(now - startTime) * (100 - percent) / percent;
				printf("\r%d%%  to go:%d secs                ", percent, secsToGo);
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
				rgb = rgb + this->trace(ray, 0);
			}
			rgb = rgb / (float)samplesPerPixel;
			rgb.clamp();
			image->putPixel(x, y, rgb);
		}
	}
	printf("\nDONE... %d sec\n", (time(NULL) - startTime));

	return image;
}

RGB Pathtracer::trace(Ray p_ray, int p_depth) {

	RGB radiance(0,0,0);
	RGB localRadiance(0,0,0);
	RayIntersection nearestRayInt;
	p_depth++;
	
	bool intersected = false;

	if(scene->kdTree == NULL) {
		intersected = p_ray.nearestIntersection(scene->getObjects(), scene->getObjectCount(), &nearestRayInt);
	} else {
		intersected = scene->kdTree->nearestIntersection(&p_ray, &nearestRayInt);
	}

	if(intersected) {
		if(!(nearestRayInt.object->getShader()->isLight() && p_ray.getDiffuse())) {
			//local ilumination:
			//Vector3d viewVec = cameraPos - nearestRayInt.point;
			Vector3d viewVec = p_ray.getDir();
			viewVec.normalize();
			localRadiance = nearestRayInt.object->getShader()->getDiffuseRadiance(nearestRayInt.object, nearestRayInt.point, nearestRayInt.normal, nearestRayInt.uv, viewVec, scene);

			radiance = localRadiance;

			int outVecsCount = 0;
			RGB indirectRadiance;

			if(p_depth == 1) {
				for(int i = 0; i < 5; i++) {
					if(p_depth < maxDepth) {
						Ray outRays[10];
						RGB values[10];
						outVecsCount = nearestRayInt.object->getShader()->getBRDFSampledRays(nearestRayInt.object, nearestRayInt.point, nearestRayInt.normal, nearestRayInt.uv, viewVec, p_ray.getDir(), outRays, values);
						
						for(int s = 0; s < outVecsCount; s++) {
							Ray ray = outRays[s];
							indirectRadiance = indirectRadiance + (trace(ray, p_depth) * values[s]) ;
						}
					}

					indirectRadiance;
				}

				indirectRadiance = indirectRadiance / 5;
			} else {
				if(p_depth < maxDepth) {
					Ray outRays[10];
					RGB values[10];
					outVecsCount = nearestRayInt.object->getShader()->getBRDFSampledRays(nearestRayInt.object, nearestRayInt.point, nearestRayInt.normal, nearestRayInt.uv, viewVec, p_ray.getDir(), outRays, values);
					
					for(int s = 0; s < outVecsCount; s++) {
						Ray ray = outRays[s];
						indirectRadiance = indirectRadiance + (trace(ray, p_depth) * values[s]) ;
					}
				}	
			}

			if(outVecsCount > 0) {
				radiance =  radiance + indirectRadiance;
			}
		}
	} else if(scene->getBackground() != NULL){ // && !p_ray.getDiffuse()) {
		radiance = scene->getBackground()->getRadianceAt(p_ray.getPoint());
	}

	return radiance;
}