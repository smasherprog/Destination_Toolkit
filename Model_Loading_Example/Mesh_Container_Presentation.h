#ifndef MESH_CONTAINER_PRESENTATION_H
#define MESH_CONTAINER_PRESENTATION_H

#include "../MY_UI/cWidgetBase.h"

class Mesh_UI;
class Base_Camera;
class Base_Mesh;
class Mesh_Container_Service;
class Mesh_Container_Presentation: public MY_UI::Controls::cWidgetBase {
public:

	Mesh_Container_Presentation(const Base_Camera* camera);
	virtual ~Mesh_Container_Presentation();

	void Draw();

	virtual void MouseMoved() override;
	virtual MY_UI::Controls::cWidgetBase* Hit() override;
	virtual void LeftMouseButtonUp()override;

protected:


	Mesh_UI* MeshUI;
	const Base_Camera* Camera;
	Mesh_Container_Service* Mesh_Container;
	Base_Mesh* Last_Hit;
};




#endif