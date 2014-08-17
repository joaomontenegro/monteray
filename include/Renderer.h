#pragma once

#include "Camera.h"
#include "Scene3d.h"
#include "Image.h"

class Renderer {
public:
	virtual Image* render() = 0;

	Scene3d* scene;
};