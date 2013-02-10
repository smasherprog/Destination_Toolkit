#ifndef UI_MESH_3D_TOOL_H
#define UI_MESH_3D_TOOL_H
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/CheckBox.h"

class mat4;
class vec3;
class Base_Mesh;
class UI_Mesh_3D_Tool : public MY_UI::Controls::cWidgetBase {
	UI_Mesh_3D_Tool();//not allowed!
public:

	UI_Mesh_3D_Tool(mat4* camera_view, mat4* camera_proj, std::function<Base_Mesh*(vec3& ray, vec3& origin)> f);
	virtual ~UI_Mesh_3D_Tool(){}
	virtual void MouseMoved() override;
	virtual MY_UI::Controls::cWidgetBase* Hit() override;
	virtual MY_UI::Controls::cWidgetBase* Hit_And_SetFocus() override;

	void Set_WorldHitFunction(std::function<Base_Mesh*(vec3& ray, vec3& origin)> f){HitChecker =f;}
	void Set_WorldMats(mat4* view, mat4* proj){ View=view; Proj = proj; }

protected:

	std::function<Base_Mesh*(vec3& ray, vec3& origin)> HitChecker;// a function that is called from this to see if a base_mesh has been hit
	mat4* View, *Proj;
};

#endif