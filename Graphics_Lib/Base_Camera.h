#ifndef BASE_CAMERA_H
#define BASE_CAMERA_H
#include "../Utilities/MyMath.h"

// this class is used to connect with the UI_Camera class so that different types of cameras can be passed to the UI_Camera class
class Base_Camera{
public:

	enum Movement_Types{ Positive=1, Negative=-1, Zero=0  };

	vec3 Position, Right, Up, Look;
	mat4 View, Proj, VP, InvView, InvProj, InvVP, InvVPToScreen;
	mat4 Trans_View, Trans_Proj, Trans_VP, Trans_InvView, Trans_InvProj, Trans_InvVP, Trans_InvVPToScreen;
	Plane Planes[6];// frustum planes... normalized, built each frame
	float NearPlane, FarPlane, Screen_Width, Screen_Height, Screen_Fov, Screen_Aspect, Speed;//speed is the movement speed of the camera
	float Look_Sentitivity;// a variable that increase the look speed

	Base_Camera();
	virtual ~Base_Camera(){}
	/*
		call PerFrame passing the difference in the time from the last frame... Delta Time
		This function may seem expensive, however, it is designed to save redundant calculations because other parts of the program can use these precalculated matrices.
	*/
	virtual void PerFrame(float dt);
	// Call this when ever the Screen resolution changes.
	virtual void OnResize(float screen_height, float screen_width);
	/*
		fieldofview is in radians
		screen_height and screen_width are in pixels
		near_plane_distance is the distance of the near clipping plan from the camera
		far_plane_distance is the distance of the far clipping plan from the camera
		This is usually called on creation to setup the viewing able, near plan and far plan
	*/
	virtual void SetLens(float fieldofview, float screen_height, float screen_width, float near_plane_distance, float far_plane_distance);
	/* 
		the movment of the below three functions does not take place until PerFrame is called. Otherwise, I would have to pass in the dt to each of these functions. 
		Doing that might cause bugs in code because one part of code might call Forward(dt), then if another part of program called it again, this would give twice the movment. 
		Doing the code this way means that you only need to make sure PerFrame is called once per frame and the rest of the program can call Forward as many times as it likes and the camera wont go Zooming around.
	*/
	virtual void Forward(Movement_Types t){ Walking= (float)t;}
	virtual void Strafe(Movement_Types t){ Strafing= (float)t;}
	virtual void Rotate(Movement_Types t){ Rotating= (float)t;}
	
	//Pitch is the amount of radians of movement ABOUT the x axis. You can visualize this as nodding your head yes. That motion is Pitch
	virtual void Pitch(float radians);
	//heading is the amount of radians of movment ABOUT the y axis. You can visual this as moving your head left to right. If you answer NO to a question using a head gesture, this is the movment of Heading.
	virtual void Heading(float radians);//heading is also known as Yaw
	
	//No yaw here because I am not creating a camera for a flight sim. This is a camera as would be for a human walking on the ground. Yaw
	//Yaw would be tilting your head to the left or to right.

	//below will be functions that are added to test bounding volumes agaist the view frustum
	virtual bool InsideFrustum(const vec3& point){ return ::InsideFrustum(point, Planes); }
	virtual bool InsideFrustum(const vec3& point, float radiussqrd){ return ::InsideFrustum(point, radiussqrd, Planes); }


protected:

	virtual void Forward(float dt){ Position +=  (Look* (dt*Speed)); }
	virtual void Strafe(float dt){Position += (Right* (dt *Speed));}
	float Walking, Strafing, Rotating;// used in the per frame

};


#endif 