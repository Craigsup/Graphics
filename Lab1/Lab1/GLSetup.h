#pragma once

#define GLEW_STATIC 

#include <Windows.h>
#include "GL\glew.h"

HDC InitGL(HWND parent);
int LoadShaders(const wchar_t* vertshader, const wchar_t* fragshader);
