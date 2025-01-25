#pragma once
#include <d3d11.h>

using drawcallback = void(void*);

//bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* device);
bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

int GuiMain(drawcallback drawfunction, void* obj_ptr);
