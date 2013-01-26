#ifndef OBJECT_3D_H
#define OBJECT_3D_H
#include "cWidgetBase.h"
#include "..\Utilities\MyMath.h"

namespace MY_UI{

	namespace Controls{

		class Translator: public cWidgetBase{
		public:

			Translator(cWidgetBase* parent);
			~Translator() {}

			virtual cWidgetBase* Hit_And_SetFocus(){ return Hit(); }
			virtual cWidgetBase* Hit();
	
			virtual void Draw_Clip() {}
			virtual void Draw_NoClip() {}
			virtual void Update();// all drawing code is in the update function to bypass having to rebuild the UI solely to update this control 

			virtual void Dragging();
			virtual void Set_Links(mat4* scal, mat4* rot, mat4* trans, mat4* proj, mat4* view, float obj_x_size, float obj_y_size, float obj_z_size);
			virtual void EditInWorldSpace(bool w){ WorldSpaceEditing = w;}

			float Obj_x_size, Obj_y_size, Obj_z_size;// the size of the object before any transformations take effect
			mat4 *Scaling, *Rotation, *Translation;
			const mat4 *Projection, *View;
			vec3 AxisHit;
			bool WorldSpaceEditing;
			
		};
#define MAX_POINTS 9
		class Rotator: public cWidgetBase{
		public:

			Rotator(cWidgetBase* parent);
			~Rotator() {}

			virtual cWidgetBase* Hit_And_SetFocus(){ return Hit(); }
			virtual cWidgetBase* Hit();
	
			virtual void Draw_Clip() {}
			virtual void Draw_NoClip() {}
			virtual void Update();// all drawing code is in the update function to bypass having to rebuild the UI solely to update this control 

			virtual void Dragging();
			// use this set links if you do not store heading pitch and bank seperatly
			virtual void Set_Links(mat4* scal, mat4* rot, mat4* trans, mat4* proj, mat4* view, float obj_x_size, float obj_y_size, float obj_z_size);
			// use this if you calculate the rotation matrix elsewhere in your code based off of a quaternion
			virtual void Set_Links(mat4* scal, mat4* rot, quat* qrot, mat4* trans, mat4* proj, mat4* view, float obj_x_size, float obj_y_size, float obj_z_size);

			//helper functions to reduce code
			void DrawLine(vec2& screens, vec3* arr, MY_UI::Utilities::Color col);
			bool CheckforHit(vec3& ray1, vec3& origin1, vec3* arr);

			float Obj_x_size, Obj_y_size, Obj_z_size;// the size of the object before any transformations take effect
			quat* QRotation;
			mat4 *Scaling, *Rotation, *Translation, *Projection, *View;
			int AxisHit;

			//The Rots below are the untransformed points that I calcuate once and store
			vec3 YRot[MAX_POINTS], XRot[MAX_POINTS], ZRot[MAX_POINTS];
			// the Last Rots below are the completly transformed points so that I can do ray testing without redoing math that has already been done to actually drawn the points
			vec3 LastYRot[MAX_POINTS], LastXRot[MAX_POINTS], LastZRot[MAX_POINTS];
		};

		class Scaler: public cWidgetBase{
		public:

			Scaler(cWidgetBase* parent);
			~Scaler() {}

			virtual cWidgetBase* Hit_And_SetFocus(){ return Hit(); }
			virtual cWidgetBase* Hit();
	
			virtual void Draw_Clip() {}
			virtual void Draw_NoClip() {}
			virtual void Update();// all drawing code is in the update function to bypass having to rebuild the UI solely to update this control 

			virtual void Dragging();
			virtual void Set_Links(mat4* scal, mat4* rot, mat4* trans, mat4* proj, mat4* view, float obj_x_size, float obj_y_size, float obj_z_size);

			float Obj_x_size, Obj_y_size, Obj_z_size;// the size of the object before any transformations take effect
			mat4 *Scaling, *Rotation, *Translation, *Projection, *View;
			vec3 AxisHit;
			
		};
	};

};

#endif