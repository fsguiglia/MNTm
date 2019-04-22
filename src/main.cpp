#include "ofMain.h"
#include "ofApp.h"
#include "../resource.h"

//========================================================================
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	ofGLFWWindowSettings settings;
	settings.setSize(1024, 480);
	settings.resizable = false;
	ofCreateWindow(settings);

	HWND hwnd = ofGetWin32Window();
	HICON hMyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAIN_ICON));
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hMyIcon);

	ofRunApp(new ofApp());
}
*/



int main( ){
	ofGLFWWindowSettings settings;
	settings.setSize(1024, 480);
	settings.resizable = false;
	ofCreateWindow(settings);
	return ofRunApp(new ofApp);
}
