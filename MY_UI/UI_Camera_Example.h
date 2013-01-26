#ifndef UI_CAMERA_EXAMPLE
#define UI_CAMERA_EXAMPLE
#include "MY_UI_Camera.h"
#include "../Utilities/MyMath.h"

// this camera does not pass a parent to the constructor. Its purpose is to be built and passed to the ApplicationLayer via   SetCamera.  After that, the ApplicationLayer will delete it upon its destruction
class UI_Camera_Example: public MY_UI_Camera {
public:

	UI_Camera_Example();
	virtual ~UI_Camera_Example();

	void pitch(int v);
	void heading(int v);
	void walk(float dt);
	void strafe(float dt);

	void DraggingCamera();
	virtual void KeyDown();
	virtual void KeyUp();

	void Update();// rebuild the camera view matrix
	mat4 mView;
	mat4 mProj;	

	vec3 Position, Up, Look, Right;

	//timer stuff
	float mSecondsPerCount;
	float mDeltaTime;
	float Rotating, Walking, Strafing;
	__int64 mPrevTime;
	__int64 mCurrTime;

};


#endif