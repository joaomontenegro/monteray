#pragma once

#include <stdio.h>
#include <string.h>

#include <direct.h> // for getcwd
#include <stdlib.h>// for MAX_PATH
// function to return the current working directory
// this is generally the application path
void GetCurrentPath(char* buffer) {
	_getcwd(buffer, _MAX_PATH);
}

const char* CONFIG_FILE = "e:\\programming\\visual_studio\\raytracer\\raytracer\\conf.txt";

class Config {
public:
	int IMAGE_WIDTH;
	int IMAGE_HEIGHT;
	int RAYTRACING_DEPTH;
	int SAMPLES_PER_PIXEL;

	Config() {
		int IMAGE_WIDTH = 300;
		int IMAGE_HEIGHT = 200;
		int RAYTRACING_DEPTH = 5;
		int SAMPLES_PER_PIXEL = 10;
	}

	void readConfFile();
};

inline void Config::readConfFile() {
	FILE * f;
	errno_t err = fopen_s(&f, "CONFIG_FILE", "r");
	char variable[100];
	int value;

	if(f == NULL) {
		char errMsg[1000];
		strerror_s(errMsg, 1000, err);
		
		char cd[1000];
		GetCurrentPath(cd);
		printf("%s\n", cd);

		printf("* Error loading config file. Using default confs: %s\n\n", errMsg);
	} else {
		for(int i = 0; i < 20; i++) {
			if(fscanf_s(f, "%s", variable) == EOF) {
				break;
			}

			if(fscanf_s(f, "%d", &value) == EOF) {
				break;
			}

			if(strcmp(variable, "width:") == 0) {
				printf("Width: " + value);
				IMAGE_WIDTH = value;
			} else if(strcmp(variable, "height:") == 0) {
				printf("Height: " + value);
				IMAGE_HEIGHT = value;
			} else if(strcmp(variable, "samples_per_pixel:") == 0) {
				printf("Sampl. PP: " + value);
				SAMPLES_PER_PIXEL = value;
			} else if(strcmp(variable, "raytrace_depth") == 0) {
				printf("RAY DEPTH: " + value);
				RAYTRACING_DEPTH = value;
			}
		}
	}

	fclose(f);
}