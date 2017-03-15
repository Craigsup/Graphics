#include <Windows.h>  // for access to the windows APIs
#include <crtdbg.h>   // for debug stuff
#include "GLSetup.h"
#include "Utils.h"
#include "Matrix.h"
#include "RenderingContext.h"

static HWND hwnd;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnCreate();
void OnDraw();
void OnTimer(UINT nIDEvent);
void OnSize(DWORD type, UINT cx, UINT cy);
void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
void OnLButtonDown(UINT nFlags, int x, int y);
void OnLButtonUp(UINT nFlags, int x, int y);
void OnMouseMove(UINT nFlags, int x, int y);
void CreateObjects();
void CleanUp();
void Picture();

#define NOOFVERTS   6

GLuint textureid1;

const int noOfSegments = 16;
float eye[3] = { 0, 0, 4 };
float center[3] = { 0,0,0 };
float up[3] = { 0,1,0 };

RenderingContext rcontext;

int glprogram, poshandle, normalhandle, diffusehandle;
int vbos[2];
const double VERTICES[NOOFVERTS][3] =
{
	{ -0.90, -0.90, 0.0 }, // Triangle 1
	{ 0.85, -0.90, 0.0 },
	{ -0.90, 0.85, 0.0 },
	{ 0.90, -0.85, 0.0 }, // Triangle 2
	{ 0.90, 0.90, 0.0 },
	{ -0.85, 0.90, 0.0 }
};
const short POLYGONS[NOOFVERTS / 3][3] =
{
	{ 0, 1, 2 },
	{ 3, 4, 5 }
};


// Win32 entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// This mini section is really useful to find memory leaks
#ifdef _DEBUG   // only include this section of code in the DEBUG build
	//  _CrtSetBreakAlloc(65);  // really useful line of code to help find memory leaks
	_onexit(_CrtDumpMemoryLeaks); // check for memory leaks when the program exits
#endif

	// To create a window, we must first define various attributes about it
	WNDCLASSEX classname;
	classname.cbSize = sizeof(WNDCLASSEX);
	classname.style = CS_HREDRAW | CS_VREDRAW;
	classname.lpfnWndProc = (WNDPROC)WndProc;   // This is the name of the event-based callback method
	classname.cbClsExtra = 0;
	classname.cbWndExtra = 0;
	classname.hInstance = hInstance;
	classname.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	classname.hCursor = LoadCursor(NULL, IDC_ARROW);
	classname.hbrBackground = NULL;
	classname.lpszMenuName = NULL;
	classname.lpszClassName = L"GettingStarted";
	classname.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	RegisterClassEx(&classname);

	// Default size will be 800x600 and we'll centre it on the screen - this include the caption and window borders so might not be the canvas size (which will be smaller)
	int width = 1024;
	int height = 768;
	int offx = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int offy = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	// Create the window using the definition provided above
	CreateWindowEx(NULL, L"GettingStarted", L"Getting Started with OpenGL", WS_OVERLAPPEDWINDOW | WS_VISIBLE, offx, offy, width, height, NULL, NULL, hInstance, NULL);

	// Set the event-based message system up
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	while (msg.message != WM_QUIT) {  // keep looping until we get the quit message
		if (GetMessage(&msg, NULL, 0, 0)) { // cause this thread to wait until there is a message to process
			// These two lines of code take the MSG structure, mess with it, and correctly dispatch it to the WndProc defined during the window creation
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CleanUp();
}

// This is our message handling method and is called by the system (via the above while-loop) when we have a message to process
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		hwnd = hWnd;
		OnCreate();
		return 0;
		break;
	case WM_SIZE:
		OnSize((DWORD)wParam, (UINT)(lParam & 0xFFFF), (UINT)(lParam >> 16));
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT paint;
		BeginPaint(hwnd, &paint);
		OnDraw();
		EndPaint(hwnd, &paint);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// This is called then the window is first created and useful to get things ready (e.g. load or create pens, brushes, images, etc)
void OnCreate()
{
	InitGL(hwnd);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		DisplayMessage((char*)glewGetErrorString(err));
	}

	// Load in shaders
	// can change between multiple shader programs
	glprogram = LoadShaders(L"vertshader.txt", L"fragshader.txt");
	
	poshandle = glGetAttribLocation(glprogram, "a_position");
	normalhandle = glGetAttribLocation(glprogram, "a_normal");

	diffusehandle = glGetUniformLocation(glprogram, "u_m_diffuse");
	rcontext.mvphandle = glGetUniformLocation(glprogram, "u_mvpmatrix");


	glUseProgram(glprogram);

	CreateObjects();

	// Background colour
	glClearColor(1.0f, 1.0f, 0.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
}

void CreateObjects() {
// Do creation here. Images, objects,meshes, etc
	// preload / precalculate here

	// euler, get tickcount at start of draw and save. already have last tick count. *never* do two gettickcounts as will lose a small amount.

	glGenBuffers(2, (unsigned int*)vbos);

	Picture();

	// each vertex has an x, y and z coordinate
	int size = sizeof(float)*6*noOfSegments;
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, size, VERTICES, GL_STATIC_DRAW);

	size = sizeof(short)*3*noOfSegments;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, POLYGONS, GL_STATIC_DRAW);
}

// This is called when the window needs to be redrawn
void OnDraw() {
	int stride = 6;
	glUseProgram(glprogram);
	rcontext.InitModelMatrix(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix::SetLookAt(rcontext.viewmatrix, eye, center, up);
	//rcontext.Translate(0.0f, 0.0f, -3.5f);

	rcontext.PushModelMatrix();
		rcontext.Translate(-1.0f, 1.0f, 0.0f);
		rcontext.RotateZ(180.0f);
		rcontext.UpdateMVPs();
		glUniform4f(diffusehandle, 1.0f, 0.5f, 0.0f, 1.0f);
		glUniformMatrix4fv(rcontext.mvphandle, 1, false, rcontext.mvpmatrix);

		glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
		glVertexAttribPointer(poshandle, 3, GL_DOUBLE, false, 0, (void*)0);
		glVertexAttribPointer(normalhandle, 3, GL_FLOAT, false, /*stride*/sizeof(float) * stride, (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(poshandle);
		glEnableVertexAttribArray(normalhandle);

		glDrawElements(GL_TRIANGLES, NOOFVERTS, GL_UNSIGNED_SHORT, 0);
	rcontext.PopModelMatrix();	
	
	rcontext.PushModelMatrix();
		rcontext.Translate(1.0f, 1.0f, 0.0f);
		rcontext.RotateZ(90.0f);
		rcontext.UpdateMVPs();
		glUniform4f(diffusehandle, 1.0f, 0.5f, 0.0f, 1.0f);
		glUniformMatrix4fv(rcontext.mvphandle, 1, false, rcontext.mvpmatrix);

		glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
		glVertexAttribPointer(poshandle, 3, GL_DOUBLE, false, 0, (void*)0);
		glVertexAttribPointer(normalhandle, 3, GL_FLOAT, false, sizeof(float) * stride, (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(poshandle);
		glEnableVertexAttribArray(normalhandle);

		glDrawElements(GL_TRIANGLES, NOOFVERTS, GL_UNSIGNED_SHORT, 0);
	rcontext.PopModelMatrix();

	rcontext.PushModelMatrix();
		rcontext.Translate(-1.0f, -1.0f, 0.0f);
		rcontext.RotateZ(270.0f);
		rcontext.UpdateMVPs();
		glUniform4f(diffusehandle, 1.0f, 0.5f, 0.0f, 1.0f);
		glUniformMatrix4fv(rcontext.mvphandle, 1, false, rcontext.mvpmatrix);

		glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
		glVertexAttribPointer(poshandle, 3, GL_DOUBLE, false, 0, (void*)0);
		glVertexAttribPointer(normalhandle, 3, GL_FLOAT, false, sizeof(float) * stride, (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(poshandle);
		glEnableVertexAttribArray(normalhandle);

		glDrawElements(GL_TRIANGLES, NOOFVERTS, GL_UNSIGNED_SHORT, 0);
	rcontext.PopModelMatrix();

	rcontext.PushModelMatrix();
		rcontext.Translate(1.0f, -1.0f, 0.0f);
		//rcontext.RotateZ(-90.0f);
		rcontext.UpdateMVPs();
		glUniform4f(diffusehandle, 1.0f, 0.5f, 0.0f, 1.0f);
		glUniformMatrix4fv(rcontext.mvphandle, 1, false, rcontext.mvpmatrix);

		glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
		glVertexAttribPointer(poshandle, 3, GL_DOUBLE, false, 0, (void*)0);
		glVertexAttribPointer(normalhandle, 3, GL_FLOAT, false, sizeof(float) * stride, (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(poshandle);
		glEnableVertexAttribArray(normalhandle);

		glDrawElements(GL_TRIANGLES, NOOFVERTS, GL_UNSIGNED_SHORT, 0);
	rcontext.PopModelMatrix();

	glFinish();
	SwapBuffers(wglGetCurrentDC());
}

void Picture() {
	BITMAP imageinfo;
	BYTE* imgdata;
	DWORD* temp, *pixeldata;
	HBITMAP image = (HBITMAP) ::LoadImage(NULL, L"sun.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	::GetObject(image, sizeof(BITMAP), &imageinfo);
	imgdata = (BYTE*)imageinfo.bmBits;

	temp = pixeldata = (DWORD*)malloc(imageinfo.bmWidth*imageinfo.bmHeight * 4);
	int i = 0;
	for (int x = 0; x < (imageinfo.bmWidth*imageinfo.bmHeight) * 3; x += 3) {
		pixeldata[i] = (DWORD)((255 << 24) | (imgdata[x] << 16) | (imgdata[x + 1] << 8) | (imgdata[x + 2]));
		i++;
	}

	glGenTextures(1, &textureid1);
	glBindTexture(GL_TEXTURE_2D, textureid1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageinfo.bmWidth, imageinfo.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp);

	glUniform1i(textureid1, 0);
	glActiveTexture(GL_TEXTURE0);




	//free(temp);
	//free(pixeldata);
}

// Called when the window is resized
void OnSize(DWORD type, UINT cx, UINT cy) {
	if (cx>0 && cy>0) {
		glViewport(0, 0, cx, cy);

		// our FOV is 60 degrees 
		const float FOVY = (60.0f*(float)M_PI / 180.0f);
		const float NEAR_CLIP = 0.01f;  // for example
		const float FAR_CLIP = 100.0f;  // for example

		float fAspect = (float)cx / cy;
		float top = NEAR_CLIP * tanf(FOVY/2);
		float bottom = -top;
		float left = fAspect*bottom;
		float right = fAspect*top;

		Matrix::SetFrustum(rcontext.projectionmatrix, left, right, bottom, top, NEAR_CLIP, FAR_CLIP);
	}
}

void CleanUp() {
	glDeleteProgram(glprogram);
	glDeleteBuffers(2, (unsigned int*)vbos);
}
