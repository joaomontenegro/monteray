#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>

#include "RGB.h"
#include "Image.h"
#include "Vector3d.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Pathtracer.h"
#include "Raytracer.h"
#include "Scene3d.h"
#include "PinholeCamera.h"
#include "PointLight.h"
#include "IBLLight.h"
#include "TriangleLight.h"
#include "LambertianShader.h"
#include "PhongShader.h"
#include "TexturedLambertianShader.h"
#include "MirrorShader.h"
#include "GlassShader.h"
#include "MRLoader.h"
#include "OBJLoader.h"

void render(Image *img);

int main(int argc, char *argv[]) {

	OBJLoader objLoader;
	//MRLoader mrloader;
	Renderer *renderer;
	
	//renderer = mrloader.loadMRFile(argv[1]);
	//renderer = objLoader.loadOBJFile(argv[1], "");
	renderer = objLoader.loadOBJFile("D:\\cg\\models\\test\\test.obj", "");

	cout << endl << endl << endl;

	if(renderer == NULL) {
		printf("Error Loading scene\n");
		exit(1);
	}

	printf("Rendering %s\n", renderer->scene->name);
	printf(" %d objects | %d lights\n", renderer->scene->objectCount, renderer->scene->lightCount);

	Image* img = renderer->render();
	if(argc > 2) {
		img->writeBMP(argv[2]);
	} else {
		img->writeBMP("test.bmp");
	}
	
	return 0;
}
