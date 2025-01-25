#pragma once
#include "CommonObjects.h"

class DrawThread
{
public:
	void operator()(CommonObjects& common);
	void CleanupTextureCache();
	//void DrawAppWindow(void* common_ptr);
};

