#include "MRLoader.h"

using namespace std;

const int MAX_OBJECTS = 2000000;
const int MAX_LIGHTS = 10;
const int MAX_SHADERS = 20;


Renderer* MRLoader::loadMRFile(char* p_filename) {
	ifstream inFile;

	Scene3d * p_scene = new Scene3d("scene");
		
	inFile.open(p_filename);
	if(!inFile) {
		cerr << "Error reading: " << p_filename << endl;
		return NULL;
	}

	char word[200];
	int wordSize = 0;

	//scene 
	Renderer* renderer = loadRenderer(p_scene, &inFile);
	if(renderer == NULL) {
		return NULL;
	}

	Object3d** objs = new Object3d*[MAX_OBJECTS];
	p_scene->setObjects(objs, 0);

	Light** lights = new Light*[MAX_LIGHTS];
	p_scene->setLights(lights, 0);

	Shader** shaders = new Shader*[MAX_SHADERS];
	int shaderCount = 0;

	bool useKDTree = false;
	bool kdVerbose;
	int kdMaxDepth;
	int kdMinObjPerNode;

	bool notFinished = true;
	while(notFinished) {
		inFile >> word;
		
		if(strcmp(word, "kd") == 0) {
			if(!loadKDTree(p_scene, &inFile, &kdMaxDepth, &kdMinObjPerNode, &kdVerbose)) {
				return NULL;
			}
			useKDTree = true;
		} else if(strcmp(word, "cam") == 0) {
			if(!loadCamera(p_scene, &inFile)) {
				return NULL;
			}
		} else if(strcmp(word, "light") == 0) {
			if(!loadLight(p_scene, &inFile)) {
				return NULL;
			}
		} else if(strcmp(word, "shader") == 0) {
			if(!loadShader(shaders, &inFile)) {
				return NULL;
			}
			shaderCount++;
		} else if(strcmp(word, "obj") == 0) {
			if(!loadObject(p_scene, shaders, shaderCount, &inFile)) {
				return NULL;
			}
		}

		if(inFile.eof()) {
			notFinished = NULL;
		}
	}
	
	inFile.close();

	if(useKDTree) {
		p_scene->initKDTree(kdMaxDepth, kdMinObjPerNode, kdVerbose);
	}

	return renderer;
}

bool MRLoader::loadKDTree(Scene3d* p_scene, ifstream* p_inFile, int* p_maxDepth, int* p_minObjPerNode, bool* p_verbose) {
	char word[200];

	*p_inFile >> word;
	if(strcmp(word, "yes") == 0) {
		*p_verbose = true;
	} else if(strcmp(word, "no") == 0) {
		*p_verbose = false;
	} else {
		cout << "After 'kd' there must be a 'yes' or 'no' for kd-tree verbosing" << endl; 
	}

	//maxDepth
	*p_inFile >> word;
	if(strcmp(word, "depth") != 0) {
		cout << "expected 'depth' argument in kd, found: |" << word <<"|"<< endl;
		return false;
	}

	*p_inFile >> *p_maxDepth;

	//minobjpernode
	*p_inFile >> word;
	if(strcmp(word, "min") != 0) {
		cout << "expected 'min' argument in kd, found: |" << word <<"|"<< endl;
		return false;
	}

	*p_inFile >> *p_minObjPerNode;

	return true;
}

Renderer* MRLoader::loadRenderer(Scene3d* p_scene, ifstream* p_inFile) {
	Renderer *renderer;

	char word[200];

	*p_inFile >> word;
	if(!strcmp(word, "scene") == 0) {
		cout << "expected 'scene' token"<< endl;
		return NULL;
	}

	*p_inFile >> word;
	if(strcmp(word, "ray") == 0) {	
		
		//samples per pixel
		*p_inFile >> word;
		if(strcmp(word, "spp") != 0) {
			cout << "expected 'spp' argument in ray, found: |" << word <<"|"<< endl;
			return NULL;
		}

		int spp;
		*p_inFile >> spp;

		//max Depth
		*p_inFile >> word;
		if(strcmp(word, "depth") != 0) {
			cout << "expected 'depth' argument in ray, found: |" << word <<"|"<< endl;
			return NULL;
		}

		int depth;
		*p_inFile >> depth;

		Raytracer* r = new Raytracer(p_scene, spp, depth);
		renderer = r;
	} else if(strcmp(word, "path") == 0) {	
		
		//samples per pixel
		*p_inFile >> word;
		if(strcmp(word, "spp") != 0) {
			cout << "expected 'spp' argument in path, found: |" << word <<"|"<< endl;
			return NULL;
		}

		int spp;
		*p_inFile >> spp;

		//max Depth
		*p_inFile >> word;
		if(strcmp(word, "depth") != 0) {
			cout << "expected 'depth' argument in path, found: |" << word <<"|"<< endl;
			return NULL;
		}

		int depth;
		*p_inFile >> depth;

		Pathtracer* p = new Pathtracer(p_scene, spp, depth);
		renderer = p;
	} else {
		cout << "not a valid renderer: " << word <<endl;
	}

	return renderer;
}

bool MRLoader::loadCamera(Scene3d* p_scene, ifstream* p_inFile) {
	char word[200];

	*p_inFile >> word;

	//pin
	if(strcmp(word, "pin") == 0) {
		*p_inFile >> word;
		float cx, cy, cz;
		float tx, ty, tz;
		float upx, upy, upz;
		float fov;
		int w, h;

		//c camera position
		*p_inFile >> word;
		if(strcmp(word, "c") != 0) {
			cout << "expected 'c' argument in pin, found: |" << word <<"|"<< endl;
			return false;
		}
		
		*p_inFile >> cx;
		*p_inFile >> cy;
		*p_inFile >> cz;
		
		//t camera target
		*p_inFile >> word;
		if(strcmp(word, "t") != 0) {
			cout << "expected 't' argument in pin, found: " << word << endl;
			return false;
		}
		
		*p_inFile >> tx;
		*p_inFile >> ty;
		*p_inFile >> tz;

		//up camera up vector
		*p_inFile >> word;
		if(strcmp(word, "up") != 0) {
			cout << "expected 'up' argument in pin, found: " << word << endl;
			return false;
		}
		
		*p_inFile >> upx;
		*p_inFile >> upy;
		*p_inFile >> upz;
		
		//fov camera fov
		*p_inFile >> word;
		if(strcmp(word, "fov") != 0) {
			cout << "expected 'fov' argument in pin, found: " << word << endl;
			return false;
		}
		
		*p_inFile >> fov;

		//wh camera width & height
		*p_inFile >> word;
		if(strcmp(word, "wh") != 0) {
			cout << "expected 'wh' argument in pin, found: " << word << endl;
			return false;
		}
		
		*p_inFile >> w;
		*p_inFile >> h;

		Vector3d c(cx, cy, cz);
		Vector3d t(tx, ty, tz);
		Vector3d up(upx, upy, upz);

		PinholeCamera* pinholeCam = new PinholeCamera(c, t, up, fov, w, h);

		p_scene->camera = pinholeCam;

	} else {
		cout << "wrong camera type: " << word << endl;
		return false;
	}

	return true;
}

bool MRLoader::loadLight(Scene3d* p_scene, ifstream* p_inFile) {
	char word[200];

	*p_inFile >> word;

	//tri
	if(strcmp(word, "tri") == 0) {
		float r, g, b;
		float v1x, v1y, v1z;
		float v2x, v2y, v2z;
		float v3x, v3y, v3z;
		
		//v1
		*p_inFile >> word;
		if(strcmp(word, "v1") != 0) {
			cout << "expected 'v1' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> v1x;
		*p_inFile >> v1y;
		*p_inFile >> v1z;

		//v2
		*p_inFile >> word;
		if(strcmp(word, "v2") != 0) {
			cout << "expected 'v2' argument in light tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> v2x;
		*p_inFile >> v2y;
		*p_inFile >> v2z;

		//v3
		*p_inFile >> word;
		if(strcmp(word, "v3") != 0) {
			cout << "expected 'v3' argument in light tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> v3x;
		*p_inFile >> v3y;
		*p_inFile >> v3z;

		//flux light flux
		*p_inFile >> word;
		if(strcmp(word, "flux") != 0) {
			cout << "expected 'flux' argument in light tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> r;
		*p_inFile >> g;
		*p_inFile >> b;

		TriangleLight* tri = new TriangleLight("trilight", Vector3d(v1x, v1y, v1z), Vector3d(v2x, v2y, v2z), Vector3d(v3x, v3y, v3z), RGB(r,g,b), new LambertianLightShader());
		
		p_scene->addLight(tri, p_scene->lightCount++);
		p_scene->addObject(&(tri->triangle), p_scene->objectCount++);

	} else if(strcmp(word, "pnt") == 0) {
		float px, py, pz;
		float r, g, b;

		//pos light position
		*p_inFile >> word;
		if(strcmp(word, "pos") != 0) {
			cout << "expected 'pos' argument in light pnt, found: " << word << endl;
			return false;
		}
		*p_inFile >> px;
		*p_inFile >> py;
		*p_inFile >> pz;

		//flux light flux
		*p_inFile >> word;
		if(strcmp(word, "flux") != 0) {
			cout << "expected 'flux' argument in light pnt, found: " << word << endl;
			return false;
		}
		*p_inFile >> r;
		*p_inFile >> g;
		*p_inFile >> b;

		PointLight *pointLight = new PointLight("light", Vector3d(px, py, pz), RGB(r, g, b));

		p_scene->addLight(pointLight, p_scene->lightCount++);

	} else if(strcmp(word, "ibl") == 0) {
		//image
		*p_inFile >> word;
		if(strcmp(word, "image") != 0) {
			cout << "expected 'image' argument in light ibl, found: " << word << endl;
			return false;
		}
		
		char filename[200];
		*p_inFile >> filename;

		//gamma
		*p_inFile >> word;
		if(strcmp(word, "gamma") != 0) {
			cout << "expected 'gamma' argument in light ibl, found: " << word << endl;
			return false;
		}

		float gamma;
		*p_inFile >> gamma;

		Image* bgimg = new Image();
		printf("Loading background image: '%s'\n", filename);
		bgimg->readBMP(filename);
		IBLLight* ibl = new IBLLight(bgimg, gamma);

		p_scene->addLight(ibl, p_scene->lightCount++);
		p_scene->setBackground(ibl);

	} else {
		cout << "wrong light type: " << word << endl;
		return false;
	}

	return true;
}

bool MRLoader::loadShader(Shader** p_shaders, ifstream* p_inFile) {
	char word[200];

	//lamb
	*p_inFile >> word;
	if(strcmp(word, "lamb") == 0) {
		int shaderN;
		*p_inFile >> shaderN;

		//refl
		*p_inFile >> word;
		if(strcmp(word, "refl") != 0) {
			cout << "expected 'refl' argument in shader lamb, found: " << word << endl;
			return false;
		}
		float r,g,b;
		
		*p_inFile >> r;
		*p_inFile >> g;
		*p_inFile >> b;

		LambertianShader* lambShader = new LambertianShader(RGB(r,g,b));
		p_shaders[shaderN] = lambShader;

	} else 	if(strcmp(word, "mirr") == 0) {
		int shaderN;
		*p_inFile >> shaderN;
		//refl
		*p_inFile >> word;
		if(strcmp(word, "refl") != 0) {
			cout << "expected 'refl' argument in shader mirr, found: " << word << endl;
			return false;
		}
		float r,g,b;

		*p_inFile >> r;
		*p_inFile >> g;
		*p_inFile >> b;

		MirrorShader* mirrShader = new MirrorShader(RGB(r,g,b));
		p_shaders[shaderN] = mirrShader;

	} else if(strcmp(word, "glass") == 0) {
		int shaderN;
		*p_inFile >> shaderN;
		//refl
		*p_inFile >> word;
		if(strcmp(word, "trans") != 0) {
			cout << "expected 'trans' argument in shader glass, found: " << word << endl;
			return false;
		}
		float r,g,b;

		*p_inFile >> r;
		*p_inFile >> g;
		*p_inFile >> b;

		*p_inFile >> word;
		if(strcmp(word, "index") != 0) {
			cout << "expected 'index' argument in shader glass, found: " << word << endl;
			return false;
		}

		float refrIndex;
		*p_inFile >> refrIndex;

		GlassShader* mirrShader = new GlassShader(RGB(r,g,b), refrIndex);
		p_shaders[shaderN] = mirrShader;

	}else if(strcmp(word, "phongglass") == 0) {
		int shaderN;
		*p_inFile >> shaderN;
		//refl
		*p_inFile >> word;
		if(strcmp(word, "trans") != 0) {
			cout << "expected 'trans' argument in shader phongglass, found: " << word << endl;
			return false;
		}
		float r,g,b;

		*p_inFile >> r;
		*p_inFile >> g;
		*p_inFile >> b;

		*p_inFile >> word;
		if(strcmp(word, "pindex") != 0) {
			cout << "expected 'pindex' argument in shader phong, found: " << word << endl;
			return false;
		}

		float phongIndex;
		*p_inFile >> phongIndex;

		*p_inFile >> word;
		if(strcmp(word, "rindex") != 0) {
			cout << "expected 'rindex' argument in shader phongglass, found: " << word << endl;
			return false;
		}

		float refrIndex;
		*p_inFile >> refrIndex;

		PhongGlassShader* mirrShader = new PhongGlassShader(RGB(r,g,b), phongIndex, refrIndex);
		p_shaders[shaderN] = mirrShader;

	}else if(strcmp(word, "phongglass") == 0) {
		int shaderN;
		*p_inFile >> shaderN;
		//refl
		*p_inFile >> word;
		if(strcmp(word, "trans") != 0) {
			cout << "expected 'trans' argument in shader phongglass, found: " << word << endl;
			return false;
		}
		float r,g,b;

		*p_inFile >> r;
		*p_inFile >> g;
		*p_inFile >> b;

		*p_inFile >> word;
		if(strcmp(word, "pindex") != 0) {
			cout << "expected 'pindex' argument in shader phong, found: " << word << endl;
			return false;
		}

		float phongIndex;
		*p_inFile >> phongIndex;

		*p_inFile >> word;
		if(strcmp(word, "rindex") != 0) {
			cout << "expected 'rindex' argument in shader phongglass, found: " << word << endl;
			return false;
		}

		float refrIndex;
		*p_inFile >> refrIndex;

		PhongGlassShader* mirrShader = new PhongGlassShader(RGB(r,g,b), phongIndex, refrIndex);
		p_shaders[shaderN] = mirrShader;
	} else if(strcmp(word, "texlamb") == 0) {
		int shaderN;
		*p_inFile >> shaderN;

		//refl
		*p_inFile >> word;
		if(strcmp(word, "img") != 0) {
			cout << "expected 'img' argument in shader texlamb, found: " << word << endl;
			return false;
		}

		char filename[250];
		*p_inFile >> filename;

		Image* img = new Image();
		if(!img->readBMP(filename)) {
			cout << "error loading image: \"" << filename << "\"" << endl;
			return false;
		}

		TexturedLambertianShader* texShader = new TexturedLambertianShader(img);
		p_shaders[shaderN] = texShader;
	} else {
		cout << "wrong shader type: " << word << endl;
		return false;
	}

	return true;
}

bool MRLoader::loadObject(Scene3d* p_scene, Shader** p_shaders, int p_shaderCount, ifstream* p_inFile) {
	char word[200];

	//tri
	*p_inFile >> word;
	if(strcmp(word, "sphere") == 0) {
		int shaderN;
		*p_inFile >> shaderN;

		//v1
		*p_inFile >> word;
		if(strcmp(word, "center") != 0) {
			cout << "expected 'center' argument in obj sphere, found: " << word << endl;
			return false;
		}

		float cx, cy, cz;
		*p_inFile >> cx;
		*p_inFile >> cy;
		*p_inFile >> cz;

		*p_inFile >> word;
		if(strcmp(word, "radius") != 0) {
			cout << "expected 'radius' argument in obj sphere, found: " << word << endl;
			return false;
		}

		float radius;
		*p_inFile >> radius;

		Vector3d center(cx, cy, cz);
		Sphere* sphere = new Sphere(center, radius, p_shaders[shaderN]);
		
		p_scene->addObject(sphere, p_scene->objectCount++);

	}else if(strcmp(word, "tri") == 0) {
		int shaderN;
		float v1x, v1y, v1z;
		float v2x, v2y, v2z;
		float v3x, v3y, v3z;
		
		float n1x, n1y, n1z;
		float n2x, n2y, n2z;
		float n3x, n3y, n3z;

		float uv1x, uv1y, uv1z;
		float uv2x, uv2y, uv2z;
		float uv3x, uv3y, uv3z;

		*p_inFile >> shaderN;

		//v1
		*p_inFile >> word;
		if(strcmp(word, "v1") != 0) {
			cout << "expected 'v1' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> v1x;
		*p_inFile >> v1y;
		*p_inFile >> v1z;

		//v2
		*p_inFile >> word;
		if(strcmp(word, "v2") != 0) {
			cout << "expected 'v2' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> v2x;
		*p_inFile >> v2y;
		*p_inFile >> v2z;

		//v3
		*p_inFile >> word;
		if(strcmp(word, "v3") != 0) {
			cout << "expected 'v3' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> v3x;
		*p_inFile >> v3y;
		*p_inFile >> v3z;

				//n1
		*p_inFile >> word;
		if(strcmp(word, "n1") != 0) {
			cout << "expected 'n1' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> n1x;
		*p_inFile >> n1y;
		*p_inFile >> n1z;

		//n2
		*p_inFile >> word;
		if(strcmp(word, "n2") != 0) {
			cout << "expected 'n2' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> n2x;
		*p_inFile >> n2y;
		*p_inFile >> n2z;

		//n3
		*p_inFile >> word;
		if(strcmp(word, "n3") != 0) {
			cout << "expected 'n3' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> n3x;
		*p_inFile >> n3y;
		*p_inFile >> n3z;

				//uv1
		*p_inFile >> word;
		if(strcmp(word, "uv1") != 0) {
			cout << "expected 'uv1' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> uv1x;
		*p_inFile >> uv1y;
		*p_inFile >> uv1z;

		//uv2
		*p_inFile >> word;
		if(strcmp(word, "uv2") != 0) {
			cout << "expected 'uv2' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> uv2x;
		*p_inFile >> uv2y;
		*p_inFile >> uv2z;

		//uv3
		*p_inFile >> word;
		if(strcmp(word, "uv3") != 0) {
			cout << "expected 'uv3' argument in obj tri, found: " << word << endl;
			return false;
		}
		*p_inFile >> uv3x;
		*p_inFile >> uv3y;
		*p_inFile >> uv3z;

		Vector3d v1(v1x, v1y, v1z);
		Vector3d v2(v2x, v2y, v2z);
		Vector3d v3(v3x, v3y, v3z);

		Vector3d n1(n1x, n1y, n1z);
		Vector3d n2(n2x, n2y, n2z);
		Vector3d n3(n3x, n3y, n3z);

		UVMap uv1; uv1.u = uv1x; uv1.v = uv1y; uv1.w = uv1z;
		UVMap uv2; uv2.u = uv2x; uv2.v = uv2y; uv2.w = uv2z;
		UVMap uv3; uv3.u = uv3x; uv3.v = uv3y; uv3.w = uv3z;

		if(shaderN >= p_shaderCount) {
			shaderN = 0;
		}

		//Triangle* tri = new Triangle(v1, v2, v3, n1, n2, n3, uv1, uv2, uv3, p_shaders[shaderN]);
		Triangle* tri = new Triangle(v1, v2, v3, uv1, uv2, uv3, p_shaders[shaderN]);

		p_scene->addObject(tri, p_scene->objectCount++);

	} else {
		cout << "wrong object type: " << word << endl;
		return false;
	}

	return true;
}