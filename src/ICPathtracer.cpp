#include "ICPathtracer.h"

Image* ICPathtracer::render() {
	Image* image = new Image(scene->camera->getWidth(), scene->camera->getHeight());

	srand((unsigned int)time(NULL));
	time_t startTime = time(NULL);
	int printGoal = (scene->camera->getWidth() * scene->camera->getHeight()) / 100;
	int count = 0;
	int percent = 0;
	time_t now = 0;
	
	printf("Irradiance Cache Pass:\n");
	BoundingBox* bbox = scene->getSceneBoundingBox();
	IrradianceCache irradianceCache(IC_A, Vector3d(0,0,0), bbox->getMaxLength(), IC_DEPTH);
	Ray ray;
	RayIntersection nearestRayInt;
	IrradianceSample irradiance;
	vector<IrradianceSample> irradiances;
	bool intersected;
	time_t startTimeIC = time(NULL);

	for(int y = scene->camera->getHeight() - 1; y >= 0 ; y--) {
		for(int x = 0; x < scene->camera->getWidth(); x++) {
			
			if(count == printGoal) {
				count = 0;
				now = time(NULL);
				percent++;
				time_t secsToGo = (int)(now - startTimeIC) * (100 - percent) / percent;
				printf("\r%d%%  to go:%d secs                ", percent, secsToGo);
			}
			count++;
			
			ray = scene->camera->castRay((float)x, (float)y);
			intersected = false;
			if(scene->kdTree == NULL) {
				intersected = ray.nearestIntersection(scene->getObjects(), scene->getObjectCount(), &nearestRayInt);
			} else {
				intersected = scene->kdTree->nearestIntersection(&ray, &nearestRayInt);
			}
	
			if(intersected) {
				irradiances.clear();
				irradianceCache.searchIrradiances(nearestRayInt.point, nearestRayInt.normal, &irradiances);
				if(irradiances.size() == 0) {
					irradiance.point = nearestRayInt.point;
					irradiance.normal = nearestRayInt.normal;
					calculateIrradiance(nearestRayInt.point, nearestRayInt.normal, &irradiance);
					irradiance.weight = 0.0f;
					OctreeNode* n = irradianceCache.addIrradianceSample(irradiance);
					//image->putPixel(x, y, RGB(0.2f, 0.2f, 0.2f) + irradiance.irradiance);
					image->putPixel(x, y, RGB(1,1,1));
					//cout << endl << x << " " << y << endl;
				} else {
					image->putPixel(x, y, RGB(0,0,0));
				}
			}			
		}
	}
	
	cout << endl << "DONE... " << irradianceCache.nSamples << " IC samples in " << (time(NULL) - startTime) << " sec" << endl;
	image->writeBMP("ic_samples.bmp");

	cout << "Render Pass: ";

	count = 0;
	percent = 0;
	float weightSum;
	vector<IrradianceSample>::iterator it;
	RGB indirect;
	RGB direct;
	RGB color;
	Vector3d normalsCross;
	Vector3d pDifference;
	float rotGradient;
	float transGradient;
	RGB radianceIncrement(0, 0, 0);
	time_t startTimeRender = time(NULL);
	for(int y = scene->camera->getHeight() - 1; y >= 0 ; y--) {
		for(int x = 0; x < scene->camera->getWidth(); x++) {
			if(count == printGoal) {
				count = 0;
				now = time(NULL);
				percent++;
				time_t secsToGo = (int)(now - startTimeRender) * (100 - percent) / percent;
				printf("\r%d%%  to go:%d secs                ", percent, secsToGo);
			}
			count++;
			
			direct = RGB(0, 0, 0);
			for(int s = 0; s < samplesPerPixel; s++) {
				
				float jitterX = 0.0f;
				float jitterY = 0.0f;

				if(s < samplesPerPixel - 1) {
					jitterX = (((float)(rand() % 1000)) / 1000.0f) - 0.5f;
					jitterY = (((float)(rand() % 1000)) / 1000.0f) - 0.5f;
				}

				ray = scene->camera->castRay((float)x + jitterX, (float)y + jitterY);
				ray.setDiffuse(false);
				direct = direct + trace(ray, &nearestRayInt, maxDepth - 1);
			}
			
			indirect = RGB(0, 0, 0);
			if(nearestRayInt.dist < FAR_AWAY) {
				irradiances.clear();
				irradianceCache.searchIrradiances(nearestRayInt.point, nearestRayInt.normal, &irradiances);
				if(irradiances.size() == 0) {
					image->putPixel(x, y, direct);
				} else {
					weightSum = 0;
					for(it = irradiances.begin(); it < irradiances.end(); ++it) {
						irradiance = *it;
						normalsCross = irradiance.normal.cross(nearestRayInt.normal);
						rotGradient = normalsCross.dot(irradiance.rotGradient);
						
						pDifference = nearestRayInt.point - irradiance.point;
						transGradient = pDifference.dot(irradiance.transGradient);
						radianceIncrement = irradiance.irradiance;
						//radianceIncrement = radianceIncrement + RGB(rotGradient, rotGradient, rotGradient);
						//radianceIncrement = radianceIncrement + RGB(transGradient, transGradient, transGradient);
						radianceIncrement = radianceIncrement / M_PI;
						indirect = indirect + (radianceIncrement * irradiance.weight);
						weightSum += irradiance.weight;
					}
					indirect = indirect / weightSum;
					indirect = indirect * nearestRayInt.object->getShader()->getDiffuseReflectance(nearestRayInt.uv);
				}
			}
			direct = direct / (float)samplesPerPixel;
			image->putPixel(x, y, indirect + direct);
		}
	}

	now = time(NULL);
	printf("\nDONE... %d sec\n", (time(NULL) - startTime));

	return image;
}

void ICPathtracer::calculateIrradiance(Vector3d p_point, Vector3d p_normal, IrradianceSample* p_irradiance) {
	//the irradiance value
	RGB irradiance(0, 0, 0);

	//incoming radiance of a sample
	RGB radianceIn;

	//harmonic average of the distance of objects to the point being processed
	float distHAvg = 0.0f;

	//transforms world coordinates into local coordinates
	// defined by the normal of the surface being processed
	ONB onb;
	onb.initFromU(p_normal);

	//M = number of sections in Theta
	float m = sqrt(IC_SAMPLES / M_PI);
	int M = (int)floor(m);

	//N = number of sections in Phi
	int N = (int)floor(m * M_PI);

	//real total of samples
	int samples = M * N;

	//number of ray hits
	int nHits = 0;
	
	//Phi related variables
	float xk;
	float phi;
	float phiMinus;
	float cosPhi, sinPhi, cosPhi2, sinPhi2;
	float cosPhiMinus2, sinPhiMinus2;

	//Theta related variables
	float xj;
	float theta;
	float cosTheta, sinTheta;
	float tanThetaNeg;
	float sinThetaMinus, sinThetaPlus;
	float cosThetaMinus2;

	//unit vectors orthogonal between themselves and the normal
	Vector3d uk, vk, vkMinus;

	//sample ray variables
	float x, y, z;
	Vector3d rayDir;
	Ray ray(p_point, Vector3d(0, 0, 0));
	RayIntersection rayInt;

	//rotational gradient variables
	float rotGradValue;
	Vector3d rotGrad;

	//translational gradient variables
	float transGradValueRight;
	float transGradValueLeft;
	Vector3d transGrad;

	//buffer that contains the previous k's radiance values
	float* previousRadiances = new float[M]; // allocated in the heap

	//buffer that contains the previous k's ray distance
	float* previousDists = new float[M]; // allocated in the heap

	//clean the last radiance and distances buffers
	for(int i = 0; i < M; ++i) {
		previousRadiances[i] = 0.0f;
		previousDists[i] = 0.0f;
	}

	//auxiliary temporary variables
	float brightness;
	float minDistK;
	float minDistJ;

	//iterate through all the pHis
	for(int k = 0; k < N; ++k) {
		//calculate phi and its trigonometric values
		//xk = (float)(rand() % 1000) / 1000.0f;
		xk = 0.5;
		xk = (k + xk) / N;
		phi = (float) (2 * M_PI * xk);
		cosPhi = (float) cos(phi);
		sinPhi = (float) sin(phi);
		cosPhi2 = cos(phi * phi);
		sinPhi2 = 1 - cosPhi2;
		phiMinus = (float) (2 * M_PI * (k / N));
		cosPhiMinus2 = cos(phiMinus);
		sinPhiMinus2 = 1 - cosPhiMinus2;

		//calculate uk = the unit vector in the Phi direction
		uk.x = -cosPhi;
		uk.y = sinPhi;
		uk.z = 0;
		//transform uk into world coordinates
		uk = onb.transform(uk);

		//calculate vk = the unit vector in the Phi + PI/2 direction
		vk.x = cosPhi2;
		vk.y = sinPhi2;
		vk.z = 0.0f;
		//transform vk into world coordinates
		vk = onb.transform(vk);

		//calculate vkMinus = the unit vector in the Phi + PI/2 direction
		vkMinus.x = cosPhiMinus2;
		vkMinus.y = sinPhiMinus2;
		vkMinus.z = 0.0f;
		//transform vk into world coordinates
		vkMinus = onb.transform(vkMinus);

		//reinitializes the gradients values
		rotGradValue = 0.0f;
		transGradValueRight = 0.0f;
		transGradValueLeft = 0.0f;

		for(int j = 0; j < M; ++j) {
			//calculate theta and its trignometric values
			//xj = (float)(rand() % 1000) / 1000.0f;
			xj = 0.5;
			xj = (j + xj) / M;
			sinTheta = (float) sqrt(xj);
			cosTheta = (float) sqrt(1.0f - xj);
			tanThetaNeg = - sinTheta / cosTheta;
			theta = asin(sinTheta);

			//calculate theta- and their trigonometric values
			sinThetaMinus = (float) sqrt(((float)j / M));
			cosThetaMinus2 = - (sinThetaMinus * sinThetaMinus);

			//calculate theta+ and their trigonometric values
			sinThetaPlus = (float) sqrt((((float)j + 1) / M));			
			
			//calculate the ray direction vector
			x = cosPhi * sinTheta;
			y = sinPhi * sinTheta;
			z = cosTheta;
			
			//transform into world coordinates
			ray.direction = onb.v * x + onb.w * y + onb.u * z;
			
			//ray trace
			radianceIn = trace(ray, &rayInt, 0);

			//add the contribution of this sample to the irradiance
			irradiance = irradiance + radianceIn;

			//update the distances to the point
			if(rayInt.hit) {
				distHAvg = distHAvg + (1 / rayInt.dist);
				nHits++;
			}

			//update the rotational gradient value;
			brightness = radianceIn.brightness();
			rotGradValue += tanThetaNeg * brightness;

			if(j > 0) {
				minDistJ = min(previousDists[j - 1], rayInt.dist);
				if(minDistJ < EPSILON) {
					minDistJ = EPSILON;
				}
				//update the left component of the translational gradient
				transGradValueLeft += ((sinThetaMinus - cosThetaMinus2) / minDistJ) * (brightness - previousRadiances[j - 1]);
			}

			if(k > 0) {
				minDistK = min(previousDists[j], rayInt.dist);
				if(minDistK < EPSILON) {
					minDistK = EPSILON;
				}
				//update the right component of the translational gradient
				transGradValueRight += ((sinThetaPlus - sinThetaMinus) / minDistK) * (brightness - previousRadiances[j]);
			}

			//updates the previous radiances and distances buffer
			previousRadiances[j] = radianceIn.brightness();
			previousDists[j] = rayInt.dist;
		}

		//updates the radients
		rotGrad = rotGrad + vk * rotGradValue;
		transGrad = transGrad + uk * (2.0f * M_PI / (float)N) * transGradValueRight;
		transGrad = transGrad + vkMinus * transGradValueLeft;
	}

	//calculate the average of the distances to the point
	//avoiding the division by zero
	if(distHAvg == 0.0 || nHits == 0) {
		distHAvg = INFINITY;
	} else {
		distHAvg /= nHits;
		distHAvg = 1 / distHAvg;
	}

	//averages the irradiance sample values
	irradiance = irradiance * M_PI / (float)samples;

	//multiplies the normalization factor to the rotational gradient
	rotGrad = rotGrad * M_PI / (float)samples;

	//set the members of p_irradiance
	p_irradiance->radius = distHAvg;
	p_irradiance->rotGradient = rotGrad;
	p_irradiance->transGradient = transGrad;
	p_irradiance->irradiance = irradiance;

	//remove the buffers allocaed in the heap
	delete previousRadiances;
	delete previousDists;
}


/*
	float m = sqrt(IC_SAMPLES / M_PI);
	float n = m * M_PI;
	int M = (int)floor(m);
	int N = (int)floor(n);
	int samples = M * N;

	RGB irradiance(0, 0, 0);
	RGB radianceIn;
	float brightness;
	float brightnessLastj = 0;
	float *brightnessLastk = new float[M];
	float distHAvg = 0;
	RayIntersection nearestRayInt;
	ONB onb;

    float phi;
    float cosPhi;
    float sinPhi;
	float cosPhi2;
    float sinPhi2;
    float sinTheta;
    float cosTheta;
	float sin2j;
	float sinj;
	float sinj1;
	float cos2j;
	float tanThetaMinus;
    float x;
    float y;
    float z;
	float xj;
	float xk;
	Vector3d uk;
	Vector3d vk;
	float rotGTemp;
	float transGTemp1;
	float transGTemp2;
	Vector3d rotG(0, 0, 0);
	Vector3d transG(0, 0, 0);

	samples = 0;
	onb.initFromU(p_normal);

	//clean
	for(int j = 0; j < M; ++j) {
		brightnessLastk[j] = 0.0f;
	}
	
	for(int k = 0; k < N; ++k) {
		xk = (float)(rand() % 1000) / 1000.0f;
		xk = (k + xk) / M;
		phi = (float) (2 * M_PI * xk);
		cosPhi = (float) cos(phi);
		sinPhi = (float) sin(phi);
		cosPhi2 = - sinPhi;
		sinPhi2 = cosPhi;

		//TODO: check if uk is vk (and vice-versa) or not
		uk.x = -cosPhi;
		uk.y = sinPhi;
		uk.z = 0;

		vk.x = cosPhi2;
		vk.y = sinPhi2;
		vk.z = 0;

		uk = onb.transform(uk);
		vk = onb.transform(vk);

		rotGTemp = 0.0f;
		transGTemp1 = 0.0f;
		transGTemp2 = 0.0f;
		for(int j = 0; j < M; ++j) {
			samples++;
			xj = (float)(rand() % 1000) / 1000.0f;
			xj = (j + xj) / N;
			
			sinTheta = (float) sqrt(xj);
			cosTheta = (float) sqrt(1.0f - xk);
			tanThetaMinus = - sinTheta / cosTheta;
			
			sin2j = (float)j / (float)M;
			sinj = sqrt(sin2j);
			sinj1 = sqrt((float)(j + 1) / (float)M);
			cos2j = 1.0f - sin2j;
	        
			x = cosPhi * sinTheta;
			y = sinPhi * sinTheta;
			z = cosTheta;

			ONB onb;
			onb.initFromU(p_normal);
			Vector3d outVec = onb.v * x + onb.w * y + onb.u * z;

			Ray ray;
			ray.setPoint(p_point);
			ray.setDir(outVec);
			
			radianceIn = trace(ray, &nearestRayInt);
			brightness = radianceIn.brightness();
			irradiance = irradiance + radianceIn;
			distHAvg = distHAvg + (1 / nearestRayInt.dist);
			
			rotGTemp = rotGTemp + tanThetaMinus * brightness;
			transGTemp1 = transGTemp1 + ((sinj * cos2j) / motion) * brightness - brightnessLastj; 
			transGTemp2 = transGTemp2 + (sinj1 - sinj) / motion * brightness - brightnessLastk[j];

			brightnessLastj = brightness;
			brightnessLastk[j] = brightness;
			//TODO:calc motion lastBrightnessj lastBrightnessk
		}

		rotG = rotG + vk * rotGTemp;

		transG = transG + uk * (2 * M_PI / N) * transGTemp1 + vk * transGTemp2;
	}

	distHAvg /= samples;
	distHAvg = 1 / distHAvg;
	rotG = rotG * (float)(M_PI / samples);

	p_irradiance->radius = distHAvg;
	p_irradiance->rotGradient = rotG;
	p_irradiance->transGradient = transG;
	p_irradiance->irradiance = irradiance / (float)samples;
}
*/


/*
RGB ICPathtracer::trace(Ray p_ray, RayIntersection* p_intersection, int p_depth) {
	RGB radiance(0,0,0);
	RayIntersection nearestRayInt;

	if(p_depth == maxDepth) {
		return RGB(0, 0, 0);
	}

	bool intersected = false;

	if(scene->kdTree == NULL) {
		intersected = p_ray.nearestIntersection(scene->getObjects(), scene->getObjectCount(), &nearestRayInt);
	} else {
		intersected = scene->kdTree->nearestIntersection(&p_ray, &nearestRayInt);
	}

	if(intersected) {
		if(!nearestRayInt.object->getShader()->isLight()) {
			//local ilumination:
			radiance = nearestRayInt.object->getShader()->getDiffuseRadiance(nearestRayInt.object, nearestRayInt.point, nearestRayInt.normal, nearestRayInt.uv, p_ray.getDir(), scene);
		}

		nearestRayInt.hit = true;

		*p_intersection = nearestRayInt;
	} else {
		nearestRayInt.dist = FAR_AWAY;
		nearestRayInt.hit = false;
		*p_intersection = nearestRayInt;
	}

	return radiance;
}
*/

RGB ICPathtracer::trace(Ray p_ray, RayIntersection* p_intersection, int p_depth) {

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

	if(!intersected) {
		nearestRayInt.dist = FAR_AWAY;
		nearestRayInt.hit = false;
		*p_intersection = nearestRayInt;
	}

	if(intersected) {
		(*p_intersection) = nearestRayInt;

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
							RayIntersection tempInt;
							indirectRadiance = indirectRadiance + (trace(ray, &tempInt, p_depth) * values[s]) ;
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
						RayIntersection tempInt;
						indirectRadiance = indirectRadiance + (trace(ray, &tempInt, p_depth) * values[s]) ;
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