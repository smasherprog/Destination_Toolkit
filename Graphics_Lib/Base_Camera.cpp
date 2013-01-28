#include "stdafx.h"
#include "Base_Camera.h"

Base_Camera::Base_Camera(){// start the camera looking down the positive z axis
	NearPlane=FarPlane=Screen_Width=Screen_Height=Screen_Fov=Screen_Aspect=0.0f;
	View.identity();
	Proj.identity();
	VP.identity();
	Trans_View.identity();
	Trans_Proj.identity();
	Trans_VP.identity();
	Trans_InvView.identity();
	Trans_InvProj.identity();
	Trans_InvVP.identity();
	Trans_InvVPToScreen.identity();
	Position= vec3(0.0f, 0.0f, 0.0f);
	Speed = 50.0f;
	Right = vec3(1.0f, 0.0f, 0.0f);
	Up = vec3(0.0f, 1.0f, 0.0f);
	Look= vec3(0.0f, 0.0f, 1.0f);
	Walking=Strafing=Rotating=0.0f;
	Look_Sentitivity=2.0f;
}

void Base_Camera::SetLens(float f, float height, float width, float nz, float fz){
	NearPlane=nz;
	FarPlane=fz;
	Screen_Fov=f;
	Base_Camera::OnResize(height, width);
}
void Base_Camera::OnResize(float h, float w){
	Screen_Height=h;
	Screen_Width=w;
	Screen_Aspect=Screen_Width/Screen_Height;
	PerFrame(0.0f);

}
void Base_Camera::PerFrame(float dt){

	Look.normalize();
	// the next two lines are necessary because there is creep in the floating point numbers that can cause the right vector to move on the y axis
	Right.y=0;// right is not allowed to have any y in it. This would cause the camera to Roll, and that is not wanted here. Force the x to be onthe x-z plane
	Right.normalize();

	Up= Cross(Look, Right);
	Up.normalize();

	Right = Cross(Up, Look);
	Right.normalize();

	View._11 = Right.x; 
	View._21 = Right.y; 
	View._31 = Right.z; 
	View._41 = -Dot(Position, Right);   

	View._12 = Up.x;
	View._22 = Up.y;
	View._32 = Up.z;
	View._42 = -Dot(Position, Up);  

	View._13 = Look.x; 
	View._23 = Look.y; 
	View._33 = Look.z; 
	View._43 = -Dot(Position, Look);

	View._14=0.0f;
	View._24=0.0f;
	View._34=0.0f;
	View._44=1.0f;

	Forward( Walking *dt );// normal walk speed
	Strafe( Strafing * dt);

	if(Rotating != 0.0f) Heading(dt *= Rotating);
	
	Proj.setupProject(Screen_Fov, Screen_Aspect, NearPlane, FarPlane);// regular camera....
	VP = View * Proj;// saves a few calculations, classes can access VP instead of mutliplying over and over
	InvView= View;
	InvView.inverse();
	InvProj=Proj;
	InvProj.inverse();
	InvVP= VP;
	InvVP.inverse();
	mat4 ScreenScale, ScreenTrans;
	ScreenScale.setupScale(2.0f/Screen_Width, -2.0f/Screen_Height, 1.0f);
	ScreenTrans.setupTranslation(-1.0f, 1.0f, 0.0f);
	InvVPToScreen = ( ScreenScale * ScreenTrans) * InvVP;
	ExtractPlanesD3D(Planes, VP, true);// build the frustum planes

	Trans_View=View;
	Trans_View.Transpose();
	Trans_Proj = Proj;
	Trans_Proj.Transpose();
	Trans_VP = VP;
	Trans_VP.Transpose();
	Trans_InvView = InvView;
	Trans_InvView.Transpose();
	Trans_InvProj = InvProj;
	Trans_InvProj.Transpose();
	Trans_InvVP = InvVP;
	Trans_InvVP.Transpose();
	Trans_InvVPToScreen = InvVPToScreen;
	Trans_InvVPToScreen.Transpose();
}


void Base_Camera::Pitch(float radians){
	mat4 R;
	radians*=Look_Sentitivity;
	R.setupRotate(Right, radians);
	Up*=R;
	Look*=R;
}
//heading and yaw are used interchangeably, and are the same
void Base_Camera::Heading(float radians){
	mat4 R;
	radians*=Look_Sentitivity;
	R.setupRotate(Up, radians);
	Look*=R;
	Right*=R;
}

