#include <Windows.h>  // for access to the windows APIs
#include <crtdbg.h>   // for debug stuff
#include "GLSetup.h"
#include "Utils.h"
#include "Matrix.h"
#include "RenderingContext.h"
#include "Model3D.h"
#include <string>

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
void RotateMagic();
void OnAnimate();
void LoadMaps();
void MultiplyMatrix(float* i, float* x);
void DrawPlatform();

#define ANIMATION_TIMER 101

RenderingContext rcontext;
int glprogram;

Model3D* model;
Object3D** modelObjects;
Object3D* tower, *platform, *side1, *side2, *side3, *side4, *sky, *bottom, *chair;

DWORD startTime, lastTime;
bool playing = false;
int rotX, rotY, lastX, lastY, slowAscend, fastAscend, jerkCounter;
float eye[3] = { 0, 0.2, 0.2 };
float center[3] = { 0, 0.55, 0 };
float up[3] = { 0, 1, 0 };
std::string stage;
float platformHeight, platformRotation, waitTime;
bool onBoardView = false;

GLuint towerid, platformid, side1id, side2id, side3id, side4id, skyid, bottomid, chairid;

// Win32 entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// This mini section is really useful to find memory leaks
#ifdef _DEBUG   // only include this section of code in the DEBUG build
	  _CrtSetBreakAlloc(150);  // really useful line of code to help find memory leaks
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
	case WM_LBUTTONDOWN:
		OnLButtonDown((UINT)wParam, (int)(lParam & 0xFFFF), (int)((lParam >> 16) & 0xFFFF));
		break;
	case WM_LBUTTONUP:
		OnLButtonUp((UINT)wParam, (int)(lParam & 0xFFFF), (int)((lParam >> 16) & 0xFFFF));
		break;
	case WM_MOUSEMOVE:
		OnMouseMove((UINT)wParam, (int)(lParam & 0xFFFF), (int)((lParam >> 16) & 0xFFFF));
		break;
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam & 0xFFFF, lParam >> 16);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		OnTimer(wParam);
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

	rcontext.lighthandles[0] = glGetUniformLocation(glprogram, "direction");
	rcontext.lighthandles[1] = glGetUniformLocation(glprogram, "halfplane");
	rcontext.lighthandles[2] = glGetUniformLocation(glprogram, "ambientColor");
	rcontext.lighthandles[3] = glGetUniformLocation(glprogram, "diffuseColor");
	rcontext.lighthandles[4] = glGetUniformLocation(glprogram, "specularColor");

	rcontext.mathandles[0] = glGetUniformLocation(glprogram, "ambientFactor");
	rcontext.mathandles[1] = glGetUniformLocation(glprogram, "diffuseFactor");
	rcontext.mathandles[2] = glGetUniformLocation(glprogram, "specularFactor");
	rcontext.mathandles[3] = glGetUniformLocation(glprogram, "shininess");

	rcontext.mvhandle = glGetUniformLocation(glprogram, "u_mvmatrix");
	rcontext.mvphandle = glGetUniformLocation(glprogram, "u_mvpmatrix");

	rcontext.verthandles[0] = glGetAttribLocation(glprogram, "a_position");
	rcontext.verthandles[1] = glGetAttribLocation(glprogram, "a_normal");
	rcontext.verthandles[2] = glGetAttribLocation(glprogram, "uv_coords");

	float L[3] = { 1, 1, 1 };
	float LPV[3] = { eye[0] + L[0], eye[1] + L[1], eye[2] + L[2] };
	float temp[3] = { eye[0] + L[0], eye[1] + L[1], eye[2] + L[2] };

	float direction[3];
	Matrix::Normalise3(LPV);
	float halfplane[3] = { temp[0] / LPV[0], temp[1] / LPV[1], temp[2] / LPV[2] };

	glUniform3f(rcontext.lighthandles[0], 1, 1, 1);
	glUniform3f(rcontext.lighthandles[1], halfplane[0], halfplane[1], halfplane[2]);
	glUniform4f(rcontext.lighthandles[2], 1.0f, 1.0f, 1.0f, 0.0f);
	glUniform4f(rcontext.lighthandles[3], 1.0f, 1.0f, 1.0f, 0.0f);
	glUniform4f(rcontext.lighthandles[4], 1.0f, 1.0f, 1.0f, 0.0f);

	glUniform4f(rcontext.mathandles[0], 1.0f, 1.0f, 1.0f, 0.0f);
	glUniform4f(rcontext.mathandles[1], 1.0f, 1.0f, 1.0f, 0.0f);
	glUniform4f(rcontext.mathandles[2], 1.0f, 1.0f, 1.0f, 0.0f);
	glUniform1f(rcontext.mathandles[3], 0.0f);

	LoadMaps();

	glUseProgram(glprogram);

	CreateObjects();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Background colour
	glClearColor(0.0f, 0.5f, 0.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
}

void CreateObjects() {
	// Do creation here. Images, objects,meshes, etc
	// preload / precalculate here

	// euler, get tickcount at start of draw and save. already have last tick count. *never* do two gettickcounts as will lose a small amount.
	model = Model3D::LoadModel(L"FinalPls.3dm");
	modelObjects = model->GetObjects();
	platform = modelObjects[0];
	tower = modelObjects[1];
	sky = modelObjects[2];
	bottom = modelObjects[2];
	side1 = modelObjects[3];
	side2 = modelObjects[4];
	side3 = modelObjects[3];
	side4 = modelObjects[4];
	chair = modelObjects[5];

	eye[0] = 0;
	eye[1] = 0.2;
	eye[2] = 1;
	center[0] = 0;
	center[1] = 0.55;
	center[2] = 0;
}

// This is called when the window needs to be redrawn
void OnDraw() {
	glUseProgram(glprogram);
	rcontext.InitModelMatrix(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (onBoardView) {
		eye[0] = 0;
		eye[1] = 0;
		eye[2] = 0;
		center[0] = 0;
		center[1] = 0;
		center[2] = 0;
	}
	else {
		/*eye[0] = 0;
		eye[1] = 0.2;
		eye[2] = 1;
		center[0] = 0;
		center[1] = 0.55;
		center[2] = 0;*/
	}

	rcontext.Scale(3.0f, 3.0f, 3.0f);
	rcontext.RotateY(0.4);
	DWORD now = ::GetTickCount();
	DWORD elapsed = now - lastTime;

	rcontext.PushModelMatrix();
		glUniform1i(towerid, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, towerid);
		tower->Draw(rcontext);
	rcontext.PopModelMatrix();

	// SKY
	rcontext.PushModelMatrix();
		glUniform1i(skyid, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skyid);
		rcontext.Translate(0, 0.9, 0);
		sky->Draw(rcontext);
	rcontext.PopModelMatrix();

	// FLOOR
	rcontext.PushModelMatrix();
		glUniform1i(bottomid, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bottomid);
		bottom->Draw(rcontext);
	rcontext.PopModelMatrix();

	// SIDE 1
	rcontext.PushModelMatrix();
		glUniform1i(side1id, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, side1id);
		rcontext.Translate(0, 0.45, 0.48);
		rcontext.RotateX(180);
		side1->Draw(rcontext);
	rcontext.PopModelMatrix();

	// SIDE 2
	rcontext.PushModelMatrix();
		glUniform1i(side1id, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, side1id);
		rcontext.Translate(0.48, 0.45, 0);
		rcontext.RotateX(180);
		rcontext.RotateY(180);
		side2->Draw(rcontext);
	rcontext.PopModelMatrix();

	// SIDE 3
	rcontext.PushModelMatrix();
		glUniform1i(side1id, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, side1id);
		rcontext.Translate(0, 0.45, -0.48);
		rcontext.RotateX(180);
		side3->Draw(rcontext);
	rcontext.PopModelMatrix();

	// SIDE 4
	rcontext.PushModelMatrix();
		glUniform1i(side1id, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, side1id);
		rcontext.Translate(-0.48, 0.45, 0);
		rcontext.RotateY(180);
		rcontext.RotateX(180);
		side4->Draw(rcontext);
	rcontext.PopModelMatrix();

	if (stage == "slowascend") {
		rcontext.PushModelMatrix();
		rcontext.RotateY(platformRotation);
		rcontext.Translate(0, platformHeight, 0);
		DrawPlatform();
		rcontext.PopModelMatrix();
		platformHeight += 0.001;
		if (platformHeight >= 0.08) {
			stage = "jerkDown";
		}
	}
	if (stage == "jerkDown") {
		rcontext.PushModelMatrix();
			rcontext.RotateY(platformRotation);
			rcontext.Translate(0, platformHeight, 0);
			DrawPlatform();
		rcontext.PopModelMatrix();
		platformHeight -= 0.002;
		if (platformHeight >= 0.070) {
			stage = "jerkUp";
		}
	}
	if (stage == "jerkUp") {
		rcontext.PushModelMatrix();
			rcontext.RotateY(platformRotation);
			rcontext.Translate(0, platformHeight, 0);
			DrawPlatform();
		rcontext.PopModelMatrix();
		platformHeight += 0.002;
		if (platformHeight >= 0.085) {
			if (jerkCounter < 5) {
				stage = "jerkDown";
				jerkCounter++;
			}
			else {
				jerkCounter = 0;
				stage = "slowRotateClockwise";
			}
		}
	}
	if (stage == "slowRotateClockwise") {
		rcontext.PushModelMatrix();
			rcontext.Translate(0, platformHeight, 0);
			platformRotation += (float)sin((3 * M_PI) / 180.0f * elapsed);
			rcontext.RotateY(platformRotation);
			DrawPlatform();
		rcontext.PopModelMatrix();
		if (platformRotation > 100) {
			stage = "slowRotateAntiClockwise";
		}
	}
	if (stage == "slowRotateAntiClockwise") {
		rcontext.PushModelMatrix();
			rcontext.Translate(0, platformHeight, 0);
			platformRotation -= (float)sin((3 * M_PI) / 180.0f * elapsed);
			rcontext.RotateY(platformRotation);
			DrawPlatform();
		rcontext.PopModelMatrix();
		if (platformRotation <= 0) {
			stage = "quickascendRotate";
		}
	}
	if (stage == "quickascendRotate") {
		rcontext.PushModelMatrix();
			rcontext.Translate(0, platformHeight, 0);
			platformRotation -= (float)sin((3 * M_PI) / 180.0f * elapsed);
			rcontext.RotateY(platformRotation);
			DrawPlatform();
		rcontext.PopModelMatrix();
		platformHeight += 0.005;
		if (platformHeight >= 0.320) {
			stage = "wait";
		}
	}
	if (stage == "wait") {
		rcontext.PushModelMatrix();
			rcontext.RotateY(platformRotation);
			rcontext.Translate(0, platformHeight, 0);
			DrawPlatform();
		rcontext.PopModelMatrix();
		if (waitTime < 15) {
			waitTime += 0.5;
		}
		else {
			waitTime = 0;
			stage = "drop";
		}
	}
	if (stage == "drop") {
		rcontext.PushModelMatrix();
			rcontext.RotateY(platformRotation);
			rcontext.Translate(0, platformHeight, 0);
			DrawPlatform();
		rcontext.PopModelMatrix();
		platformHeight -= 0.025;
		if (platformHeight <= 0.05) {
			stage = "stop1";
		}
	}
	if (stage == "stop1") {
		rcontext.PushModelMatrix();
			rcontext.RotateY(platformRotation);
			rcontext.Translate(0, platformHeight, 0);
			DrawPlatform();
		rcontext.PopModelMatrix();
		if (waitTime < 20) {
			waitTime += 0.5;
		}
		else {
			waitTime = 0;
			stage = "slowascendRotate";
		}
	}
	if (stage == "slowascendRotate") {
		rcontext.PushModelMatrix();
			rcontext.Translate(0, platformHeight, 0);
			platformRotation -= (float)sin((3 * M_PI) / 180.0f * elapsed);
			rcontext.RotateY(platformRotation);
			DrawPlatform();
		rcontext.PopModelMatrix();
		platformHeight += 0.001;
		if (platformHeight >= 0.320) {
			stage = "wait1";
		}
	}
	if (stage == "wait1") {
		rcontext.PushModelMatrix();
			rcontext.RotateY(platformRotation);
			rcontext.Translate(0, platformHeight, 0);
			DrawPlatform();
		rcontext.PopModelMatrix();
		if (waitTime < 25) {
			waitTime += 0.5;
		}
		else {
			//platformHeight = 0.1;
			waitTime = 0;
			stage = "drop1";
		}
	}
	if (stage == "drop1") {
		rcontext.PushModelMatrix();
			platformRotation -= (float)sin((3 * M_PI) / 180.0f * elapsed) * 3;
			rcontext.RotateY(platformRotation);
			rcontext.Translate(0, platformHeight, 0);
			DrawPlatform();
		rcontext.PopModelMatrix();
		platformHeight -= 0.025;
		if (platformHeight <= 0.05) {
			stage = "wait2";
		}
	}
	if (stage == "wait2") {
		rcontext.PushModelMatrix();
			rcontext.RotateY(platformRotation);
			rcontext.Translate(0, platformHeight, 0);
			DrawPlatform();
		rcontext.PopModelMatrix();
		if (waitTime < 25) {
			waitTime += 0.5;
		}
		else {
			waitTime = 0;
			stage = "slowascend";
		}
	}

	if (onBoardView) {
		float i[16];
		Matrix::SetIdentity(i);
		Matrix::RotateY(i, platformRotation);
		Matrix::Translate(i, 0.01, (platformHeight*3.8), 0.35);
		MultiplyMatrix(i, eye);
		Matrix::Translate(i, 0, 0, 1);
		MultiplyMatrix(i, center);
	}

	Matrix::SetLookAt(rcontext.viewmatrix, eye, center, up);
	
	glFinish();
	SwapBuffers(wglGetCurrentDC());
	lastTime = now;
}

void DrawPlatform() {
	glUniform1i(platformid, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, platformid);
	platform->Draw(rcontext);
	int spacing = 360 / 6;
		glUniform1i(chairid, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, chairid);
		rcontext.PushModelMatrix();
		for (int i = 0; i < 360; i+=60) {
			rcontext.RotateY(i + platformRotation);
			chair->Draw(rcontext);

		}
		rcontext.PopModelMatrix();
}

void MultiplyMatrix(float* i, float* x) {
	float temp[3];
	temp[0] = (x[0] * i[0]) + (x[1] * i[4]) + (x[2] * i[8]) + i[12];
	temp[1] = (x[0] * i[1]) + (x[1] * i[5]) + (x[2] * i[9]) + i[13];
	temp[2] = (x[0] * i[2]) + (x[1] * i[6]) + (x[2] * i[10]) + i[14];

	x[0] = temp[0];
	x[1] = temp[1];
	x[2] = temp[2];

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
		float top = NEAR_CLIP * tanf(FOVY / 2);
		float bottom = -top;
		float left = fAspect*bottom;
		float right = fAspect*top;

		Matrix::SetFrustum(rcontext.projectionmatrix, left, right, bottom, top, NEAR_CLIP, FAR_CLIP);
	}
}

void CleanUp() {
	delete model;
	glDeleteProgram(glprogram);
}

void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	switch (nChar) {
		// Space Bar
	case 32:
		OnAnimate();
		break;
	case 38:
		eye[1] += 0.5;
		OnDraw();
		break;
	case 40:
		eye[1] -= 0.5;
		OnDraw();
		break; 
	case 107:
		eye[2] -= 0.1;
		OnDraw();
		break;
	case 109:
		eye[2] += 0.1;
		OnDraw();
		break;
	//reset
	case 82:
		eye[0] = 0;
		eye[1] = 0.2;
		eye[2] = 1;
		center[0] = 0;
		center[1] = 0.55;
		center[2] = 0;
		up[0] = 0;
		up[1] = 1;
		up[2] = 0;
		OnDraw();
		break;
	//top
	case 84:
		eye[0] = 0;
		eye[1] = 1.5;
		eye[2] = 0;
		center[0] = 0;
		center[1] = -0.55;
		center[2] = 0;
		up[0] = 0;
		up[1] = 1;
		up[2] = 0;
		OnDraw();
		break;
		// Back Space
	case 8:
		onBoardView = !onBoardView;
		if (onBoardView) {
			eye[2] = 0.5;
			center[0] = 0;
			center[1] = 0;
			center[2] = 1;
		}
		else {
			center[0] = 0;
			center[1] = 0.55;
			center[2] = 0;
			eye[0] = 0;
			eye[1] = 0.2;
			eye[2] = 1;
		}
		break;

		// Esc
	case 27:
		exit(0);

	default:
		_RPT1(0, "Key=%d\n", nChar);
		break;
	}
}

void OnTimer(UINT nIDEvent) {
	if (nIDEvent == ANIMATION_TIMER) {
		OnDraw();
	}
}

void OnAnimate() {
	if (playing) {
		startTime = ::GetTickCount();
		KillTimer(hwnd, ANIMATION_TIMER);
		SetWindowText(hwnd, L"Magical fairground ride - Stopped!");
		playing = false;
	}
	else {
		startTime = ::GetTickCount();
		stage = "slowascend";
		SetTimer(hwnd, ANIMATION_TIMER, 30, NULL);
		SetWindowText(hwnd, L"Magical fairground ride - Playing!");
		playing = true;
	}
}

// when the left mouse button is pressed down
void OnLButtonDown(UINT nFlags, int x, int y) {
	lastX = x;
	lastY = y;
}

// when the left mouse button is released
void OnLButtonUp(UINT nFlags, int x, int y) {

}

void OnMouseMove(UINT nFlags, int x, int y) {
	if (nFlags&MK_LBUTTON) {
		int dx = lastX - x;
		int dy = lastY - y;

		rotX = dy;
		rotY = dx;

		RotateMagic();
		lastX = x;
		lastY = y; 
	}
}

void RotateMagic() {
	float identity[16];

	Matrix::SetIdentity(identity);

	Matrix::RotateY(identity, rotY);
 	Matrix::RotateX(identity, rotX);

	//Matrix::Translate();
	
	MultiplyMatrix(identity, eye);
	MultiplyMatrix(identity, up);
}


void LoadMaps() {
	BITMAP imageinfo;
	BYTE* imgdata;
	DWORD*pixeldata;
	HBITMAP image = (HBITMAP) ::LoadImage(NULL, L"metalTexture.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	::GetObject(image, sizeof(BITMAP), &imageinfo);
	imgdata = (BYTE*)imageinfo.bmBits;

	pixeldata = (DWORD*)malloc(imageinfo.bmWidth*imageinfo.bmHeight * 4);
	int i = 0;
	for (int x = 0; x < (imageinfo.bmWidth*imageinfo.bmHeight) * 3; x += 3) {
		pixeldata[i] = (DWORD)((255 << 24) | (imgdata[x] << 16) | (imgdata[x + 1] << 8) | (imgdata[x + 2]));
		i++;
	}

	glGenTextures(1, &towerid);
	glBindTexture(GL_TEXTURE_2D, towerid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageinfo.bmWidth, imageinfo.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);

	glUniform1i(towerid, 0);
	free(pixeldata);


	// --------------------------------------------------------------------------------------------------------------------------------------------------
	image = (HBITMAP) ::LoadImage(NULL, L"sky.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	::GetObject(image, sizeof(BITMAP), &imageinfo);
	imgdata = (BYTE*)imageinfo.bmBits;

	pixeldata = (DWORD*)malloc(imageinfo.bmWidth*imageinfo.bmHeight * 4);
	i = 0;
	for (int x = 0; x < (imageinfo.bmWidth*imageinfo.bmHeight) * 3; x += 3) {
		pixeldata[i] = (DWORD)((255 << 24) | (imgdata[x] << 16) | (imgdata[x + 1] << 8) | (imgdata[x + 2]));
		i++;
	}

	glGenTextures(1, &skyid);
	glBindTexture(GL_TEXTURE_2D, skyid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageinfo.bmWidth, imageinfo.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);

	glUniform1i(skyid, 1);
	free(pixeldata);

	// --------------------------------------------------------------------------------------------------------------------------------------------------
	image = (HBITMAP) ::LoadImage(NULL, L"grass.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	::GetObject(image, sizeof(BITMAP), &imageinfo);
	imgdata = (BYTE*)imageinfo.bmBits;

	pixeldata = (DWORD*)malloc(imageinfo.bmWidth*imageinfo.bmHeight * 4);
	i = 0;
	for (int x = 0; x < (imageinfo.bmWidth*imageinfo.bmHeight) * 3; x += 3) {
		pixeldata[i] = (DWORD)((255 << 24) | (imgdata[x] << 16) | (imgdata[x + 1] << 8) | (imgdata[x + 2]));
		i++;
	}

	glGenTextures(1, &bottomid);
	glBindTexture(GL_TEXTURE_2D, bottomid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageinfo.bmWidth, imageinfo.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);

	glUniform1i(bottomid, 2);
	free(pixeldata);

	// --------------------------------------------------------------------------------------------------------------------------------------------------
	image = (HBITMAP) ::LoadImage(NULL, L"forest.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	::GetObject(image, sizeof(BITMAP), &imageinfo);
	imgdata = (BYTE*)imageinfo.bmBits;

	pixeldata = (DWORD*)malloc(imageinfo.bmWidth*imageinfo.bmHeight * 4);
	i = 0;
	for (int x = 0; x < (imageinfo.bmWidth*imageinfo.bmHeight) * 3; x += 3) {
		pixeldata[i] = (DWORD)((255 << 24) | (imgdata[x] << 16) | (imgdata[x + 1] << 8) | (imgdata[x + 2]));
		i++;
	}

	glGenTextures(1, &side1id);
	glBindTexture(GL_TEXTURE_2D, side1id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageinfo.bmWidth, imageinfo.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);

	glUniform1i(side1id, 3);
	free(pixeldata);

	/*// --------------------------------------------------------------------------------------------------------------------------------------------------
	image = (HBITMAP) ::LoadImage(NULL, L"forest.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	::GetObject(image, sizeof(BITMAP), &imageinfo);
	imgdata = (BYTE*)imageinfo.bmBits;

	pixeldata = (DWORD*)malloc(imageinfo.bmWidth*imageinfo.bmHeight * 4);
	i = 0;
	for (int x = 0; x < (imageinfo.bmWidth*imageinfo.bmHeight) * 3; x += 3) {
		pixeldata[i] = (DWORD)((255 << 24) | (imgdata[x] << 16) | (imgdata[x + 1] << 8) | (imgdata[x + 2]));
		i++;
	}

	glGenTextures(1, &side2id);
	glBindTexture(GL_TEXTURE_2D, side2id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageinfo.bmWidth, imageinfo.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);

	glUniform1i(side2id, 4);
	free(pixeldata);
	*/
	// --------------------------------------------------------------------------------------------------------------------------------------------------
	image = (HBITMAP) ::LoadImage(NULL, L"platformtexture.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	::GetObject(image, sizeof(BITMAP), &imageinfo);
	imgdata = (BYTE*)imageinfo.bmBits;

	pixeldata = (DWORD*)malloc(imageinfo.bmWidth*imageinfo.bmHeight * 4);
	i = 0;
	for (int x = 0; x < (imageinfo.bmWidth*imageinfo.bmHeight) * 3; x += 3) {
		pixeldata[i] = (DWORD)((255 << 24) | (imgdata[x] << 16) | (imgdata[x + 1] << 8) | (imgdata[x + 2]));
		i++;
	}

	glGenTextures(1, &platformid);
	glBindTexture(GL_TEXTURE_2D, platformid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageinfo.bmWidth, imageinfo.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);

	glUniform1i(platformid, 5);
	free(pixeldata);
	
	// --------------------------------------------------------------------------------------------------------------------------------------------------
	image = (HBITMAP) ::LoadImage(NULL, L"leather.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	::GetObject(image, sizeof(BITMAP), &imageinfo);
	imgdata = (BYTE*)imageinfo.bmBits;

	pixeldata = (DWORD*)malloc(imageinfo.bmWidth*imageinfo.bmHeight * 4);
	i = 0;
	for (int x = 0; x < (imageinfo.bmWidth*imageinfo.bmHeight) * 3; x += 3) {
		pixeldata[i] = (DWORD)((255 << 24) | (imgdata[x] << 16) | (imgdata[x + 1] << 8) | (imgdata[x + 2]));
		i++;
	}

	glGenTextures(1, &chairid);
	glBindTexture(GL_TEXTURE_2D, chairid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageinfo.bmWidth, imageinfo.bmHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);

	glUniform1i(chairid, 6);
	free(pixeldata);

}