#ifndef CBASED3D_H
#define CBASED3D_H
#include <string>
#include "Globals_GraphicsCore.h"
#include "GraphicsCore.h"


class cBaseD3D{
public:

	cBaseD3D(HINSTANCE hInstance, std::string appname = "My_Application" , unsigned int height = 768, unsigned int width = 1024);
	virtual ~cBaseD3D();

	virtual LRESULT msgProc(unsigned int msg, WPARAM wParam, LPARAM lParam);

	void end();
	bool begin();// if begin returns false, it means the program should terminate..

	HINSTANCE	AppInst;
	HWND		MainWnd;
	int			StartingHeight, StartingWidth;// used to make sure the window does not go below the starting size
	bool		mAppPaused, mMinimized, mMaximized, mResizing;
	int			Current_Height, Current_Width;// when a user is dragging the resize bars I do not call OnResize, but store the dimensions until the user stops dragging. 
};

#endif