#include "OBJLoader.h"

Renderer* OBJLoader::loadOBJFile(char* p_OBJFilename, char* p_MRSFilename) {
	vertices.clear();
	normals.clear();
	uvs.clear();
	faces.clear();
	mtlNames.clear();
	shaders.clear();

	//gets the OBJ file directory path
	string objFilename = p_OBJFilename;
	int pos = (int)objFilename.find_last_of("/");
	if(pos < 0) {
		pos = (int)objFilename.find_last_of("\\") + 1;
	}

	string objDir = objFilename.substr(0, pos);

	fstream fileInOBJ; //the input file stream
	string lineBuffer; //will contain one line of the text files at a time
	unsigned long int lineNumber; //the line number
	
	//opens the OBJ file
	fileInOBJ.open(p_OBJFilename,ios::in);
	if (!fileInOBJ.is_open()) {
		cout << "File : " << p_OBJFilename << " Not found " << endl;
		return NULL;
	}
	
	//scrolls through all the lines of the OBJ file
	lineNumber = 1;
	while(!fileInOBJ.eof()) {
		getline(fileInOBJ, lineBuffer);
		//cout << lineNumber << " " << lineBuffer << endl;

		//parses the line
		if(!parseOBJLine(&lineBuffer, lineNumber)) {
			fileInOBJ.close();
			return NULL;
		}
		lineNumber++;
	}
	fileInOBJ.close();

	//tests if there is a mtl file
	if(mtlFilename.compare("") != 0) {
		//if so append the directory at the begining of the filename
		mtlFilename = objDir + mtlFilename;

		cout << "mtl file: " << mtlFilename << endl;

		//open the mtl file
		fstream fileInMTL; //the input file stream
		fileInMTL.open((char *)mtlFilename.c_str(), ios::in);
		if (!fileInMTL.is_open()) {
			cout << "File : " << mtlFilename << " Not found " << endl;
			return NULL;
		}
		
		//scrolls through the mtl file lines
		lineNumber = 1;
		while(!fileInMTL.eof()) {
			getline(fileInMTL, lineBuffer);
	
			//parses the line
			if(!parseMTLLine(&lineBuffer, lineNumber)) {
				fileInMTL.close();
				return NULL;
			}
			lineNumber++;
		}
		fileInMTL.close();
	}

	//returns the created Renderer with the scene attached
	return createRenderer(p_MRSFilename);
}

bool OBJLoader::parseOBJLine(string* lineBuffer, int lineNumber) {
	stringstream ss(*lineBuffer, ios_base::in);
	string type;

	ss >> type;

	if(type[0] == '#') {
		//cout << lineNumber << ": comment" << endl;
		return true;
	}
	
	if(type.compare("v") == 0) {
		Vector3d v;
		ss >> v.x >> v.y >> v.z;
		//cout << lineNumber << " found v: " << v.x << " " << v.y << " " << v.z << endl; 
		vertices.push_back(v);
	} else if(type.compare("vn") == 0) {
		Vector3d vn;
		ss >> vn.x >> vn.y >> vn.z;
		//cout << lineNumber << " found vn: " << v.x << " " << v.y << " " << v.z << endl; 
		normals.push_back(vn);
	} else if(type.compare("vt") == 0) {
		UVMap vt;
		ss >> vt.u >> vt.v;
		//cout << lineNumber << " found vt: " << vt.u << " " << vt.v << endl; 
		uvs.push_back(vt);
	} else if(type.compare("g") == 0) {
		mtlId = -1;
	} else if(type.compare("usemtl") == 0) {
		string mtlName;
		ss >> mtlName;
		mtlId = getMtlId(mtlName);

		if(mtlId == -1) {
			mtlNames.push_back(mtlName);
			mtlId = (int)mtlNames.size() - 1;
			LambertianShader *defaultShader = new LambertianShader(RGB(0.9f, 0.9f, 0.9f));
			shaders.push_back((Shader*)defaultShader);
		}
		//cout << lineNumber << " found usemtl: " << mtlName << " " << mtlId << endl; 

	} else if(type.compare("f") == 0) {
		OBJ_f f;

		char slash;

		ss >> f.v0 >> slash >> f.vt0 >> slash >> f.vn0;
		ss >> f.v1 >> slash >> f.vt1 >> slash >> f.vn1;
		ss >> f.v2 >> slash >> f.vt2 >> slash >> f.vn2;

		/*
		cout << lineNumber << " found f: ";
		cout << f.v0 << "*" << f.vt0 << "*" << f.vn0;
		cout << f.v1 << "*" << f.vt1 << "*" << f.vn1 << " ";
		cout << f.v2 << "*" << f.vt2 << "*" << f.vn2 << endl;
		*/

		f.mtlId = mtlId;
		faces.push_back(f);
	} else if(type.compare("mtllib") == 0) {
		ss >> mtlFilename;
	}

	return true;
}

bool OBJLoader::parseMTLLine(string* lineBuffer, int lineNumber) {

	stringstream ss(*lineBuffer, ios_base::in);
	string type;

	ss >> type;

	if(type[0] == '#') {
		cout << lineNumber << " [MTL]: comment" << endl;
		return true;
	}

	if(type.compare("newmtl") == 0) {
		string mtlName;
		ss >> mtlName;
		mtlId = getMtlId(mtlName);
		if(mtlId > -1) {
			currentMtlName = mtlName;
			TexturedLambertianShader* shader = new TexturedLambertianShader();
			shaders[mtlId] = shader;
		} else {
			currentMtlName = "";
		}

		cout << lineNumber << ": new material: " << mtlName << endl;

		return true;
	}

	if(mtlId == -1) { 
		//if material not used then ignore until another newmtl is read
		return true;
	}
	
	if(type.compare("Kd") == 0) {
		RGB kd;
		ss >> kd.r >> kd.g >> kd.b;
		((TexturedLambertianShader*)shaders[mtlId])->color = kd;

		cout << lineNumber << " Kd: " <<  kd.r << kd.g << kd.b << endl;
	}

	if(type.compare("map_Kd") == 0) {
		string mapFilename;
		ss >> mapFilename;

		Image* map = new Image();
		if(!map->readBMP((char *)mapFilename.c_str())) {
			cout << "[MTL parser] error loading image: \"" << mapFilename << "\" in material \"" << currentMtlName << "\", using diffuse color instead" << endl;
		} else {
			((TexturedLambertianShader*)shaders[mtlId])->map = map;
		}

		cout << lineNumber << " map:" << mapFilename << endl;
	}	

	return true;
}

int OBJLoader::getMtlId(string mtlName) {
	vector<string>::iterator it;
	int pos = 0;
	for(it = mtlNames.begin(); it != mtlNames.end(); it++) {
		if(mtlName.compare(*it) == 0) {
			return pos;
		}
		pos++;
	}

	//not found
	return -1;
}

Renderer* OBJLoader::createRenderer(char* p_MRSFilename) {
	Scene3d* scene = new Scene3d(p_MRSFilename);
	int spp = 1;
	int maxDepth = 1;
	
	//transforms vertices vector into an array of Object3d
	Object3d** objects = new Object3d*[faces.size()];
	int objectsCount = 0;
	Vector3d v0, v1, v2;
	Vector3d n0, n1, n2;
	UVMap uv0, uv1, uv2;
	Shader* shader;
	OBJ_f face;

	vector<OBJ_f>::iterator it;
	for(it = faces.begin(); it != faces.end(); it++) {
		face = *it;

		v0 = vertices[face.v0 - 1];
		v1 = vertices[face.v1 - 1];
		v2 = vertices[face.v2 - 1];

		n0 = normals[face.vn0 - 1];
		n1 = normals[face.vn1 - 1];
		n2 = normals[face.vn2 - 1];

		uv0 = uvs[face.vt0 - 1];
		uv1 = uvs[face.vt1 - 1];
		uv2 = uvs[face.vt2 - 1];

		shader = shaders[face.mtlId];

		Triangle* triangle = new Triangle(v0, v1, v2, n0, n1, n2, uv0, uv1, uv2, shader);
		objects[objectsCount++] = triangle;
	}

	/*Sphere* sphere = new Sphere(Vector3d(0, 0.1, 0), 0.1, shaders[0]);
	objects[objectsCount++] = sphere;
	*/

	scene->setObjects(objects, objectsCount);

	PinholeCamera* camera = new PinholeCamera(Vector3d(-2.0f, 0.5f, 0.0), Vector3d(0,0.5f,0), Vector3d(0,1,0), 55, 800, 600);
	//PinholeCamera* camera = new PinholeCamera(Vector3d(0.0f, 0.35f, 0.0), Vector3d(0,0,0), Vector3d(1,0,0), 65, 400, 400);
	//PinholeCamera* camera = new PinholeCamera(Vector3d(-10, 7, 0), Vector3d(0, 0, 0), Vector3d(0,1,0), 45, 500, 500);
	scene->camera = (Camera*)camera;

	scene->initKDTree(10, 10, true);

	PointLight* light = new PointLight("pnt", Vector3d(0, 0.95f, 0), RGB(17, 17, 17));
	//PointLight* light = new PointLight("pnt", Vector3d(-20, 20.0f, 20), RGB(10000, 10000, 10000));

	Light** lights = new Light*[1];
	lights[0] = (Light*)light;
	scene->setLights(lights, 1);

	return new Pathtracer(scene, spp, maxDepth);
	//return new ICPathtracer(scene, spp, maxDepth);
}
