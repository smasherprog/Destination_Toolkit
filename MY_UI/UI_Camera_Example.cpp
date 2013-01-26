#include "stdafx.h"
#include "UI_Camera_Example.h"

#define MOVESPEED 30.0f

UI_Camera_Example::~UI_Camera_Example(){

}
UI_Camera_Example::UI_Camera_Example(){
	OnRightDrag.Add(std::bind(&UI_Camera_Example::DraggingCamera, this));

	Position= vec3(0.0f, 0.0f, -5.0f);
	Up = vec3(0.0f, 1.0f, 0.0f);
	Look= vec3(0.0f, 0.0f, 1.0f);
	Right = vec3(1.0f, 0.0f, 0.0f);

	mView.identity();
	// get the window dimensions
	mProj.identity();
		// init timer
	mSecondsPerCount=mDeltaTime=0.0f;
	mPrevTime=mCurrTime=0;
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0f / (float)countsPerSec;
	Rotating =Walking=Strafing=0.0f;
	
	MY_UI::Utilities::Point size = MY_UI::Internal::RootWidget->GetSize();
	float aspect = (float)size.x/(float)size.y;
	mProj.setupProject(0.25f*Pi, aspect, 1.0f, 1000.0f);
	Update();
}

void UI_Camera_Example::DraggingCamera(){
	pitch(Delta_Mousey);
	heading(Delta_Mousex);
}
void UI_Camera_Example::strafe(float dt){//this only changes the position, not direction
	Position += (Right* (dt *MOVESPEED));
}
void UI_Camera_Example::walk(float dt){//this only changes the position, not direction
	Position +=  (Look* (dt*MOVESPEED));
}
void UI_Camera_Example::heading(int v){
	mat4 R;
	float d = (float)v;
	d/=50.0f;
	R.setupRotateY(d);
	Right*=R;
	Up*=R;
	Look*=R;
}
void UI_Camera_Example::pitch(int v){
	mat4 R;
	float d = (float)v;
	d/=50.0f;
	R.setupRotate(Right, d);
	Look*=R;
	Up*=R;
}
void UI_Camera_Example::KeyDown(){// no need to call the lower version, this  has no need for that functionality
	mat4 R;
	switch(Current_Key){
	case(KEY_Q): // strafing left
		Strafing =-1.0f;
		break;
	case(KEY_E):  // strafing right
		Strafing =1.0f;
		break;
	case(KEY_W): 
	case(KEY_UP): // forward 
		Walking = 1.0f;
		break;
	case(KEY_S): 
	case(KEY_DOWN): // backwards
		Walking = -1.0f;
		break;
	case(KEY_A): 
	case(KEY_LEFT): // left
		if(Mouse_RButton_Down)	
			Strafing =-1.0f;
		else {
			Rotating = -1.0f;
		}
		break;
	case(KEY_D): 
	case(KEY_RIGHT): // right..
		if(Mouse_RButton_Down) {
			Strafing = 1.0f;
		}else {
			Rotating =1.0f;
		}
		break;
	case(KEY_SPACE): // jumpinggg
		break;
	};

}

void UI_Camera_Example::KeyUp(){
	switch(Current_Key){
	case(KEY_Q): // strafing left
		Strafing =0.0f;
		break;
	case(KEY_E):  // strafing right
		Strafing =0.0f;
		break;
	case(KEY_W): 
	case(KEY_UP): // forward 
		Walking = 0.0f;
		break;
	case(KEY_S): 
	case(KEY_DOWN): // backwards
		Walking = 0.0f;
		break;
	case(KEY_A): 
	case(KEY_LEFT): // left
		Strafing =  Rotating = 0.0f;
		break;
	case(KEY_D): 
	case(KEY_RIGHT): // right..
		Strafing =  Rotating = 0.0f;
		break;
	case(KEY_SPACE): // jumpinggg
		break;
	};
}

void UI_Camera_Example::Update(){
	// normally, your application should have a time keeping class for this stuff, but I place it here anyway
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	// Time difference between this frame and the previous.
	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;
	// Prepare for next frame.
	mPrevTime = mCurrTime;
	// preform any rotations here
	walk( Walking * mDeltaTime);
	strafe( Strafing * mDeltaTime);

	if(Rotating != 0.0f){
		float dt = mDeltaTime * Rotating;
		mat4 R;
		R.setupRotateY(dt);
		Right*=R;
		Up*=R;
		Look*=R;
	}

	//rebuild the view matrix here
	Look.normalize();

	Up= Cross(Look, Right);
	Up.normalize();

	Right = Cross(Up, Look);
	Right.normalize();

	mView._11 = Right.x; 
	mView._21 = Right.y; 
	mView._31 = Right.z; 
	mView._41 = -Dot(Position, Right);   

	mView._12 = Up.x;
	mView._22 = Up.y;
	mView._32 = Up.z;
	mView._42 = -Dot(Position, Up);  

	mView._13 = Look.x; 
	mView._23 = Look.y; 
	mView._33 = Look.z; 
	mView._43 = -Dot(Position, Look);

	mView._14=0.0f;
	mView._24=0.0f;
	mView._34=0.0f;
	mView._44=1.0f;


}

