#include "PCH.h"
#include "Objects_3d.h"
#include "cWidgetSkin.h"

MY_UI::Controls::Translator::Translator(cWidgetBase *parent)  : cWidgetBase(parent) {
	SetControlBounds(Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetClientBounds(Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	 Scaling=Rotation=Translation=0;
	Projection=View=0;
	 MY_UI::Internal::RootWidget->CheckForUpdate(this);
	 SetDraggable(true);// allow the widget to be dragged
	 AxisHit = vec3(0, 0, 0);
	 OnLeftDrag.Add(std::bind(&MY_UI::Controls::Translator::Dragging, this));
	 WorldSpaceEditing=false;
}

void MY_UI::Controls::Translator::Update(){
	if(GetHidden()) return;
	mat4 world;
	// sometimes it is easier to edit in world space coords rather than in object space
	if(WorldSpaceEditing){
		world=*Scaling**Translation;// looks funny doesnt it?
	} else {
		world=*Scaling**Rotation**Translation;// looks funny doesnt it?
	}
	vec3 zaxis(0.0f, 0.0f, Obj_z_size/2.0f);
	vec3 yaxis(0.0f, Obj_y_size/2.0f, 0.0f);
	vec3 xaxis(Obj_x_size/2.0f, 0.0f, 0.0f);
	vec3 center(0.0f, 0.0f, 0.0f);
	// transform the points into world space
	
		
	zaxis*=world;
	yaxis*=world;
	xaxis*=world;
	center*=world;
	

	Utilities::Point screensize = MY_UI::Internal::RootWidget->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
	// now transform the points into screen space
	vec2 e = Project(zaxis, *View, *Projection, screens.x, screens.y);
	vec2 b = Project(center, *View, *Projection, screens.x, screens.y);

	auto skin = MY_UI::Internal::WidgetSkin; 

	Utilities::Point end(static_cast<int>(e.x), static_cast<int>(e.y));
	Utilities::Point beg(static_cast<int>(b.x), static_cast<int>(b.y));
	//Z AXIS
	
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::White, MY_UI::Utilities::Red, true);
	
	e = Project(yaxis, *View, *Projection, screens.x, screens.y);
	end.x=static_cast<int>(e.x);
	end.y=static_cast<int>(e.y);
	//Y AXIS
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::White, MY_UI::Utilities::Blue);
	e = Project(xaxis, *View, *Projection, screens.x, screens.y);
	end.x=static_cast<int>(e.x);
	end.y=static_cast<int>(e.y);
	//X AXIS
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::White, MY_UI::Utilities::Green);
	MY_UI::Internal::WidgetRenderer->DrawBack(true);
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Translator::Hit(){
	if(GetHidden()) return 0;
	// convert a 2d mouse coord to a 3d ray
	vec3 ray1, origin1;// the normalized ray in world space
	auto root = MY_UI::Internal::RootWidget;
	Utilities::Point screensize = root->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
	vec2 mousepos(static_cast<float>(New_MousePosx), static_cast<float>(New_MousePosy));
	UnProject(mousepos, *View, *Projection, screens.x, screens.y, ray1, origin1);
	mat4 world;
	// sometimes it is easier to edit in world space coords rather than in object space
	if(WorldSpaceEditing){
		world=*Scaling**Translation;// looks funny doesnt it?
	} else {
		world=*Scaling**Rotation**Translation;// looks funny doesnt it?
	}
	vec3 origin2(0.0f, 0.0f, 0.0f);
	origin2*=world;// transform the point

	vec3 ray2(1.0f, 0.0f, 0.0f);
	TransformNormal(ray2, world);
	vec2 DistToIntersection = RayRayIntersect(ray1, origin1, Obj_x_size, ray2, origin2 );
	if( DistToIntersection.x != INFINITY ){

		vec3 temp = origin1 + (DistToIntersection.x*ray1);// the closest point of intersection
		vec3 temp1 = origin2 + (DistToIntersection.y*ray2);// same as above
		if(CloseTo(temp, temp1)){
			AxisHit = vec3(1.0f, 0.0f, 0.0f);
			OUTPUT_DEBUG_MSG("Hit x");
			return this;
		}

	}
	ray2 = vec3(0.0f, 1.0f, 0.0f);
	TransformNormal(ray2, world);
	DistToIntersection = RayRayIntersect(ray1, origin1, Obj_y_size,  ray2, origin2 );
	if( DistToIntersection.x != INFINITY ){
		vec3 temp = origin1 + (DistToIntersection.x*ray1);// the closest point of intersection
		vec3 temp1 = origin2 + (DistToIntersection.y*ray2);// same as above
		if(CloseTo(temp, temp1)){
			AxisHit= vec3(0.0f, 1.0f, 0.0f);
			OUTPUT_DEBUG_MSG("Hit y");
			return this;
		}
	}
	ray2 = vec3(0.0f, 0.0f, 1.0f);
	TransformNormal(ray2, world);
	DistToIntersection = RayRayIntersect(ray1, origin1, Obj_z_size,  ray2, origin2 );
	if( DistToIntersection.x != INFINITY ){
		vec3 temp = origin1 + (DistToIntersection.x*ray1);// the closest point of intersection
		vec3 temp1 = origin2 + (DistToIntersection.y*ray2);// same as above
		if(CloseTo(temp, temp1)){
			AxisHit = vec3(0.0f, 0.0f, 1.0f);
			OUTPUT_DEBUG_MSG("Hit z");
			return this;
		}
	}
	DistToIntersection=vec2(0.0f, 0.0f);
	AxisHit = vec3(0.0f, 0.0f, 0.0f);
	return 0;
}
void MY_UI::Controls::Translator::Set_Links(mat4* scal, mat4* rot, mat4* trans, mat4* proj, mat4* view, float obj_x_size, float obj_y_size, float obj_z_size){
	assert(scal != 0);
	assert(rot != 0);
	assert(trans != 0);
	assert(view != 0);
	assert(proj != 0);
	Scaling = scal;
	Rotation = rot;
	Translation = trans;
	Projection = proj;
	View = view;

	// obj size should be the MAX size of the object in that dimention before any transformations take place. What I do is create an AABB from the Untransformed mesh, 
	//then I transform the AABB by the world matrix to move it around and follow the object correctly. This initial size should remain constant through the program run-time so it only needs to be set once
	//which is why I pass them in by value
	Obj_x_size = obj_x_size;
	Obj_y_size = obj_y_size;
	Obj_z_size = obj_z_size;
}
void MY_UI::Controls::Translator::Dragging(){
	
	vec3 ray1, origin1;// the normalized ray in world space
	auto root = MY_UI::Internal::RootWidget;
	Utilities::Point screensize = root->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
	vec2 mousepos(static_cast<float>(New_MousePosx), static_cast<float>(New_MousePosy));

	UnProject(mousepos, *View, *Projection, screens.x, screens.y, ray1, origin1);

	mat4 world;
	// sometimes it is easier to edit in world space coords rather than in object space
	if(WorldSpaceEditing){
		world=*Scaling**Translation;// looks funny doesnt it?
	} else {
		world=*Scaling**Rotation**Translation;// looks funny doesnt it?
	}

	vec3 origin2(0.0f, 0.0f, 0.0f);
	origin2*=world;// transform the point
	vec3 ray2(AxisHit);
	TransformNormal(ray2, world);

	vec2 intersectiondist = RayRayIntersect(ray1, origin1, ray2, origin2);
	if(intersectiondist.y ==INFINITY) intersectiondist.y =0;// dont get all crazy now!
	vec3 ray = ray2 *intersectiondist.y;// scale the ray
	ray += origin2;// now we have the new point 


	vec2 oldmousepos(static_cast<float>(Old_MousePosx), static_cast<float>(Old_MousePosy));
	UnProject(oldmousepos, *View, *Projection, screens.x, screens.y, ray1, origin1);

	// figure out where our last point was
	intersectiondist = RayRayIntersect(ray1, origin1, ray2, origin2);
	if(intersectiondist.y ==INFINITY)intersectiondist.y =0;// dont get all crazy now!
	vec3 oldray = ray2 *intersectiondist.y;// scale the ray
	oldray += origin2;// now we have the old point 

	ray -= oldray;// ray is now the delta amount of movement... add this onto the objects position to get correct translation
	vec3 oldpos(Translation->_41, Translation->_42, Translation->_43);

	Translation->setupTranslation(oldpos + ray);
}






MY_UI::Controls::Rotator::Rotator(cWidgetBase *parent)  : cWidgetBase(parent) {
	SetControlBounds(Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetClientBounds(Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	 Scaling=Rotation=Translation=Projection=View=0;
	 MY_UI::Internal::RootWidget->CheckForUpdate(this);
	 SetDraggable(true);// allow the widget to be dragged
	 AxisHit = 0;
	 QRotation=0;
	 OnLeftDrag.Add(std::bind(&MY_UI::Controls::Rotator::Dragging, this));
	 
	float splits = 1.0f/(MAX_POINTS -1.0f);
	for(int i =0; i< MAX_POINTS; i++){
		ZRot[i].z=0.0f;
		ZRot[i].y=sinf((float)(i)*splits*Pi);
		ZRot[i].x=cosf((float)(i)*splits*Pi);

		XRot[i].x=0.0f;
		XRot[i].y=sinf((float)(i)*splits*Pi);
		XRot[i].z=cosf((float)(i)*splits*Pi);

		YRot[i].y=0.0f;
		YRot[i].x=sinf((float)(i)*splits*Pi);
		YRot[i].z=cosf((float)(i)*splits*Pi);
	}

}

void MY_UI::Controls::Rotator::Update(){
	if(GetHidden()) return;
	mat4 world(*Scaling**Rotation**Translation);// looks funny doesnt it?
	
	for(int i =0; i< 9; i++){
		LastYRot[i]=(Obj_y_size/2.0f)*YRot[i]*world;
		LastXRot[i]=(Obj_x_size/2.0f)*XRot[i]*world;
		LastZRot[i]=(Obj_z_size/2.0f)*ZRot[i]*world;
	}
	
	Utilities::Point screensize = MY_UI::Internal::RootWidget->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));

	vec2 e;
	Utilities::Point end, beg;
	// manually do the first two points here to get the correct beg and end
	e = Project(LastZRot[0], *View, *Projection, screens.x, screens.y);
	beg.x=static_cast<int>(e.x);
	beg.y=static_cast<int>(e.y);
	e = Project(LastZRot[1], *View, *Projection, screens.x, screens.y);
	end.x=static_cast<int>(e.x);
	end.y=static_cast<int>(e.y);
	//Z AXIS
	auto skin = MY_UI::Internal::WidgetSkin; 
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::Red, MY_UI::Utilities::Red, true);
	beg = end;

	for(int i =2; i< MAX_POINTS; i++) {
		e = Project(LastZRot[i], *View, *Projection, screens.x, screens.y);
		end.x=static_cast<int>(e.x);
		end.y=static_cast<int>(e.y);
		//Y AXIS
		MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::Red, MY_UI::Utilities::Red);
		beg = end;
	}
	DrawLine(screens, LastYRot, MY_UI::Utilities::Blue);
	DrawLine(screens, LastXRot, MY_UI::Utilities::Green);

	MY_UI::Internal::WidgetRenderer->DrawBack(true);
}

void MY_UI::Controls::Rotator::DrawLine(vec2& screens, vec3* arr, MY_UI::Utilities::Color col){
	auto skin = MY_UI::Internal::WidgetSkin; 
	vec2 e;
	Utilities::Point end, beg;
	// the first point needs to be calculated before the loop
	e = Project(arr[0], *View, *Projection, screens.x, screens.y);
	beg.x=static_cast<int>(e.x);
	beg.y=static_cast<int>(e.y);
	for(int i =1; i< MAX_POINTS; i++) {
		e = Project(arr[i], *View, *Projection, screens.x, screens.y);
		end.x=static_cast<int>(e.x);
		end.y=static_cast<int>(e.y);
		//Y AXIS
		MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, col, col);
		beg = end;
	}
}
bool MY_UI::Controls::Rotator::CheckforHit(const vec3& ray1, const vec3& origin1, const vec3* arr, const float scale){
	for(int i=0; i <MAX_POINTS-1; i++){// run once for each pair so one less run
		vec3 origin2 = arr[i];// beginning point
		vec3 ray2 = arr[i + 1] - arr[i];// ray between begin and end points
		float length = ray2.Length();// need to the length of the ray
		float invlength = 1.0f/length;
		ray2*=invlength;// normalize the ray
		vec2 DistToIntersection = RayRayIntersect(ray1, scale*origin1, length, ray2, scale*origin2 );
		if( DistToIntersection.x != INFINITY ){
			vec3 temp = scale*origin1 + (DistToIntersection.x*ray1);// the closest point of intersection
			vec3 temp1 = scale*origin2 + (DistToIntersection.y*ray2);// same as above
			if(CloseTo(temp, temp1)){
				return true;
			}
		}
	}
	return false;
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Rotator::Hit(){
	if(GetHidden()) return 0;
	// convert a 2d mouse coord to a 3d ray
	vec3 ray1, origin1;// the normalized ray in world space
	auto root = MY_UI::Internal::RootWidget;
	Utilities::Point screensize = root->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
	vec2 mousepos(static_cast<float>(New_MousePosx), static_cast<float>(New_MousePosy));
	UnProject(mousepos, *View, *Projection, screens.x, screens.y, ray1, origin1);
	mat4 world(*Scaling**Rotation**Translation);// looks funny doesnt it?
	

	if(CheckforHit(ray1, origin1, LastXRot, Obj_x_size/2.0f)) {
		AxisHit = 1;// 1 for x!!
		return this;
	}
	
	if(CheckforHit(ray1, origin1, LastYRot, Obj_y_size/2.0f)) {
		AxisHit = 2;
		return this;
	}
	
	if(CheckforHit(ray1, origin1, LastZRot, Obj_z_size/2.0f)) {
		AxisHit = 3 ;
		return this;
	}
	AxisHit = 0;
	return 0;
}
void MY_UI::Controls::Rotator::Set_Links(mat4* scal, mat4* rot, mat4* trans, mat4* proj, mat4* view, float obj_x_size, float obj_y_size, float obj_z_size){
	assert(scal != 0);
	assert(rot != 0);
	assert(trans != 0);
	assert(view != 0);
	assert(proj != 0);
	Scaling = scal;
	Rotation = rot;
	Translation = trans;
	Projection = proj;
	View = view;

	// obj size should be the MAX size of the object in that dimention before any transformations take place. What I do is create an AABB from the Untransformed mesh, 
	//then I transform the AABB by the world matrix to move it around and follow the object correctly. This initial size should remain constant through the program run-time so it only needs to be set once
	//which is why I pass them in by value
	Obj_x_size = obj_x_size;
	Obj_y_size = obj_y_size;
	Obj_z_size = obj_z_size;
}
void MY_UI::Controls::Rotator::Set_Links(mat4* scal, mat4* rot, quat* qrot, mat4* trans, mat4* proj, mat4* view, float obj_x_size, float obj_y_size, float obj_z_size){
	Set_Links(scal, rot, trans, proj, view, obj_x_size, obj_y_size, obj_z_size);
	assert(qrot != 0);
	QRotation = qrot;
}
void MY_UI::Controls::Rotator::Dragging(){
	if(AxisHit ==0) return;// no axis hit get out
	auto root = MY_UI::Internal::RootWidget;
	Utilities::Point screensize = root->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
	vec2 newmousepos(static_cast<float>(New_MousePosx), static_cast<float>(New_MousePosy));
	vec2 oldmousepos(static_cast<float>(Old_MousePosx), static_cast<float>(Old_MousePosy));

	//transform the center of the object into screen space
	vec2 screenpoint = Project(vec3(Translation->_41, Translation->_42, Translation->_43), *View, *Projection, screens.x, screens.y);
	// transform the mousepositions into the space of the center of the object in screen space
	vec2 oldmtri(screenpoint.x-oldmousepos.x, oldmousepos.y-screenpoint.y);
	vec2 newmtri(screenpoint.x-newmousepos.x, newmousepos.y-screenpoint.y);
	
	// get the signs of the legs
	union helper{
		float in;
		int out;
	};
	helper oldsign, newsign;
	oldsign.in = oldmtri.y;
	newsign.in = newmtri.y;
	
	if((oldsign.out&0x80000000) != (newsign.out&0x80000000)){// always go with the newsign, otherwise, the oldsign can be positive and the new negative, leading to a shift to the next quadrant
		oldsign.out = (oldsign.out&0x7fffffff)| newsign.out&0x80000000;// clear the bite, then or it with the sign in newsign
		oldmtri.y = oldsign.in;
	}
	oldsign.in = oldmtri.x;
	newsign.in = newmtri.x;
	if((oldsign.out&0x80000000) != (newsign.out&0x80000000)){// always go with the newsign, otherwise, the oldsign can be positive and the new negative, leading to a shift to the next quadrant
		oldsign.out = (oldsign.out&0x7fffffff)| newsign.out&0x80000000;// clear the bite, then or it with the sign in newsign
		oldmtri.x = oldsign.in;
	}
	//now, use good old trig to get the angle of the before mouse and the after mouse
	float old_ang = atanf(oldmtri.y/oldmtri.x);
	float new_ang = atanf(newmtri.y/newmtri.x);
	// subtract the two angles to get the delta, or difference
	
	float angcreated = new_ang - old_ang;
	
	mat4 rot;
	quat qrot;
	switch(AxisHit){
		case(1)://xaxis hit  GREEN
			if(QRotation != 0) qrot.setToRotateAboutX(angcreated);		
			else rot.setupRotateX(angcreated);
			break;
		case(2)://yaxis hit BLUE
			if(QRotation != 0) qrot.setToRotateAboutY(angcreated);		
			else rot.setupRotateY(angcreated);
			break;
		case(3)://zaxis hit  RED
			if(QRotation != 0) qrot.setToRotateAboutZ(angcreated);		
			else rot.setupRotateZ(angcreated);
			break;

	};

	if(QRotation == 0) *Rotation = rot**Rotation;
	else *QRotation *= qrot;
	
	
}







MY_UI::Controls::Scaler::Scaler(cWidgetBase *parent)  : cWidgetBase(parent) {
	SetControlBounds(Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetClientBounds(Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	 Scaling=Rotation=Translation=Projection=View=0;
	 MY_UI::Internal::RootWidget->CheckForUpdate(this);
	 SetDraggable(true);// allow the widget to be dragged
	 AxisHit = vec3(0, 0, 0);
	 OnLeftDrag.Add(std::bind(&MY_UI::Controls::Scaler::Dragging, this));
}

void MY_UI::Controls::Scaler::Update(){
	if(GetHidden()) return;
	mat4 world(*Scaling**Rotation**Translation);// looks funny doesnt it?
	vec3 zaxis(0.0f, 0.0f,  Obj_z_size);
	vec3 yaxis(0.0f, Obj_y_size, 0.0);
	vec3 xaxis(Obj_x_size, 0.0f, 0.0f);
	vec3 center(0.0f, 0.0f, 0.0f);
	// transform the points into world space
	

	Utilities::Point screensize = MY_UI::Internal::RootWidget->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
	// now transform the points into screen space
	zaxis*=world;
	vec2 e = Project(zaxis, *View, *Projection, screens.x, screens.y);
	center*=world;
	vec2 b = Project(center, *View, *Projection, screens.x, screens.y);

	auto skin = MY_UI::Internal::WidgetSkin; 

	Utilities::Point end(static_cast<int>(e.x), static_cast<int>(e.y));
	Utilities::Point beg(static_cast<int>(b.x), static_cast<int>(b.y));
	Utilities::Point temp = beg;
	//Z AXIS
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::White, MY_UI::Utilities::Red, true);
	//Y AXIS
	yaxis*=world;
	e = Project(yaxis, *View, *Projection, screens.x, screens.y);
	end.x=static_cast<int>(e.x);
	end.y=static_cast<int>(e.y);

	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::White, MY_UI::Utilities::Blue);
		
	xaxis*=world;
	e = Project(xaxis, *View, *Projection, screens.x, screens.y);
	end.x=static_cast<int>(e.x);
	end.y=static_cast<int>(e.y);
	//X AXIS
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::White, MY_UI::Utilities::Green);


	xaxis=vec3(0.25f, 0.0f, 0.0f);
	yaxis=vec3(0.0f, 0.25f, 0.0f);
	xaxis*=world;
	yaxis*=world;
	e = Project(xaxis, *View, *Projection, screens.x, screens.y);
	b = Project(yaxis, *View, *Projection, screens.x, screens.y);
	end.x=static_cast<int>(e.x);
	end.y=static_cast<int>(e.y);
	beg.x=static_cast<int>(b.x);
	beg.y=static_cast<int>(b.y);
	
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 2, MY_UI::Utilities::Blue, MY_UI::Utilities::Blue);
	

	zaxis=vec3(0.0f, 0.0f, 0.25f);
	yaxis=vec3(0.0f, 0.25f, 0.0f);
	zaxis*=world;
	yaxis*=world;
	e = Project(zaxis, *View, *Projection, screens.x, screens.y);
	b = Project(yaxis, *View, *Projection, screens.x, screens.y);
	end.x=static_cast<int>(e.x);
	end.y=static_cast<int>(e.y);
	beg.x=static_cast<int>(b.x);
	beg.y=static_cast<int>(b.y);
	
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 2, MY_UI::Utilities::Red, MY_UI::Utilities::Red);

	zaxis=vec3(0.0f, 0.0f, 0.25f);
	xaxis=vec3(0.25f, 0.0f, 0.0f);
	zaxis*=world;
	xaxis*=world;
	e = Project(zaxis, *View, *Projection, screens.x, screens.y);
	b = Project(xaxis, *View, *Projection, screens.x, screens.y);
	end.x=static_cast<int>(e.x);
	end.y=static_cast<int>(e.y);
	beg.x=static_cast<int>(b.x);
	beg.y=static_cast<int>(b.y);
	
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 2, MY_UI::Utilities::Green, MY_UI::Utilities::Green);
	
	MY_UI::Internal::WidgetRenderer->DrawBack(true);
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Scaler::Hit(){
	if(GetHidden()) return 0;
	// convert a 2d mouse coord to a 3d ray
	vec3 ray1, origin1;// the normalized ray in world space
	auto root = MY_UI::Internal::RootWidget;
	Utilities::Point screensize = root->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
	vec2 mousepos(static_cast<float>(New_MousePosx), static_cast<float>(New_MousePosy));
	UnProject(mousepos, *View, *Projection, screens.x, screens.y, ray1, origin1);
	mat4 world(*Scaling**Rotation**Translation);// looks funny doesnt it?
	vec3 origin2(0.0f, 0.0f, 0.0f);
	origin2*=world;// transform the point

	vec3 ray2(1.0f, 0.0f, 0.0f);
	TransformNormal(ray2, world);
	vec2 DistToIntersection = RayRayIntersect(ray1, origin1, Obj_x_size, ray2, origin2 );
	if( DistToIntersection.x != INFINITY ){

		vec3 temp = origin1 + (DistToIntersection.x*ray1);// the closest point of intersection
		vec3 temp1 = origin2 + (DistToIntersection.y*ray2);// same as above
		if(CloseTo(temp, temp1)){
			AxisHit = vec3(1.0f, 0.0f, 0.0f);
			return this;
		}

	}
	ray2 = vec3(0.0f, 1.0f, 0.0f);
	TransformNormal(ray2, world);
	DistToIntersection = RayRayIntersect(ray1, origin1, Obj_y_size,  ray2, origin2 );
	if( DistToIntersection.x != INFINITY ){
		vec3 temp = origin1 + (DistToIntersection.x*ray1);// the closest point of intersection
		vec3 temp1 = origin2 + (DistToIntersection.y*ray2);// same as above
		if(CloseTo(temp, temp1)){
			AxisHit= vec3(0.0f, 1.0f, 0.0f);
			return this;
		}
	}
	ray2 = vec3(0.0f, 0.0f, 1.0f);
	TransformNormal(ray2, world);
	DistToIntersection = RayRayIntersect(ray1, origin1, Obj_z_size,  ray2, origin2 );
	if( DistToIntersection.x != INFINITY ){
		vec3 temp = origin1 + (DistToIntersection.x*ray1);// the closest point of intersection
		vec3 temp1 = origin2 + (DistToIntersection.y*ray2);// same as above
		if(CloseTo(temp, temp1)){
			AxisHit = vec3(0.0f, 0.0f, 1.0f);
			return this;
		}
	}
	DistToIntersection=vec2(0.0f, 0.0f);
	AxisHit = vec3(0.0f, 0.0f, 0.0f);
	return 0;
}
void MY_UI::Controls::Scaler::Set_Links(mat4* scal, mat4* rot, mat4* trans, mat4* proj, mat4* view, float obj_x_size, float obj_y_size, float obj_z_size){
	assert(scal != 0);
	assert(rot != 0);
	assert(trans != 0);
	assert(view != 0);
	assert(proj != 0);
	Scaling = scal;
	Rotation = rot;
	Translation = trans;
	Projection = proj;
	View = view;

	// obj size should be the MAX size of the object in that dimention before any transformations take place. What I do is create an AABB from the Untransformed mesh, 
	//then I transform the AABB by the world matrix to move it around and follow the object correctly. This initial size should remain constant through the program run-time so it only needs to be set once
	//which is why I pass them in by value
	Obj_x_size = obj_x_size;
	Obj_y_size = obj_y_size;
	Obj_z_size = obj_z_size;
}
void MY_UI::Controls::Scaler::Dragging(){

	vec3 ray1, origin1;// the normalized ray in world space
	auto root = MY_UI::Internal::RootWidget;
	Utilities::Point screensize = root->GetSize();
	vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
	vec2 mousepos(static_cast<float>(New_MousePosx), static_cast<float>(New_MousePosy));

	UnProject(mousepos, *View, *Projection, screens.x, screens.y, ray1, origin1);

	mat4 world(*Scaling**Rotation**Translation);// looks funny doesnt it?

	vec3 origin2(0.0f, 0.0f, 0.0f);
	origin2*=world;// transform the point
	vec3 ray2(AxisHit);
	TransformNormal(ray2, world);

	vec2 intersectiondist = RayRayIntersect(ray1, origin1, ray2, origin2);
	if(intersectiondist.y ==INFINITY)intersectiondist.y =0;// dont get all crazy now!
	vec3 ray = ray2 *intersectiondist.y;// scale the ray
	ray += origin2;// now we have the new point 


	vec2 oldmousepos(static_cast<float>(Old_MousePosx), static_cast<float>(Old_MousePosy));
	UnProject(oldmousepos, *View, *Projection, screens.x, screens.y, ray1, origin1);

	// figure out where our last point was
	intersectiondist = RayRayIntersect(ray1, origin1, ray2, origin2);
	if(intersectiondist.y ==INFINITY)intersectiondist.y =0;// dont get all crazy now!
	vec3 oldray = ray2 *intersectiondist.y;// scale the ray
	oldray += origin2;// now we have the old point 

	ray -= oldray;// ray is now the delta amount of movement... add this onto the objects position to get correct translation
	vec3 oldpos(Scaling->_11, Scaling->_22, Scaling->_33);
	ray*=AxisHit;

	Scaling->setupScale(oldpos + ray);
}

