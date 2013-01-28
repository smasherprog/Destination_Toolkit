#ifndef UI_WORLDSELECTOR_H
#define UI_WORLDSELECTOR_H
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/CheckBox.h"

//forward declares
namespace MY_UI{
	namespace Controls{
		class Translator;
		class Rotator;
		class Scaler;
		class CheckBox;
		class Simple_Window;
	}
}
class mat4;
class vec3;
class Base_Mesh;
class UI_WorldSelector : public MY_UI::Controls::cWidgetBase {
	UI_WorldSelector();//not allowed!
public:

	UI_WorldSelector(mat4* camera_view, mat4* camera_proj, std::function<Base_Mesh*(vec3& ray, vec3& origin)> f);
	virtual ~UI_WorldSelector(){}
	virtual void MouseMoved() override;
	virtual MY_UI::Controls::cWidgetBase* Hit() override;
	virtual MY_UI::Controls::cWidgetBase* Hit_And_SetFocus() override;

	void Set_WorldHitFunction(std::function<Base_Mesh*(vec3& ray, vec3& origin)> f){HitChecker =f;}
	void Set_WorldMats(mat4* view, mat4* proj){ View=view; Proj = proj; }

protected:

	void OpenWindow();

	void CreateRotator();
	void CreateTranslator();
	void CreateScaler();
	void ClearControls();

	MY_UI::Controls::Simple_Window* Window;
	MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::CheckBox>* Trans_Checkbox;

	MY_UI::Controls::Translator*  ObjectTranslation;
	MY_UI::Controls::Rotator*  ObjectRotator;
	MY_UI::Controls::Scaler*  ObjectScaler;

	std::function<Base_Mesh*(vec3& ray, vec3& origin)> HitChecker;// a function that is called from this to see if a base_mesh has been hit

	Base_Mesh* Selected, *Last_Selected;
	mat4* View, *Proj;
};

#endif