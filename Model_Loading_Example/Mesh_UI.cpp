#include "stdafx.h"
#include "Mesh_UI.h"
#include "../MY_UI/Window.h"
#include "../MY_UI/Button.h"
#include "../MY_UI/File_Selector.h"
#include "../Graphics_Lib/Static_Mesh.h"
#include "../Graphics_Lib/Animated_Mesh.h"

Mesh_UI::Mesh_UI(){
	FileExtentions = "*.*";// all files

	MY_UI::Controls::Button* Button = new MY_UI::Controls::Button(MY_UI::Internal::RootWidget);
	Button->SetPos(10, 60);
	Button->SetText("Load Static Mesh", CENTER);
	Button->SetSizeToContents();
	Button->SetPopup("Click here to load a static mesh");

	Button->OnLeftClicked.Add(std::bind(&Mesh_UI::buttonclicked_static, this));

	MY_UI::Controls::Button* Button0 = new MY_UI::Controls::Button(MY_UI::Internal::RootWidget);
	Button0->SetText("Load Animated Mesh", CENTER);
	Button0->SetSizeToContents();
	Button0->SetPopup("Click here to load an animated mesh");

	Button0->Align_To_Widget(Button, IN_LINE | BOTTOM, 5);
	Button0->OnLeftClicked.Add(std::bind(&Mesh_UI::buttonclicked_animated, this));
}
Mesh_UI::~Mesh_UI(){
	MY_UI::Safe_Delete(FileSelector);
}
void Mesh_UI::buttonclicked_static(){
	MY_UI::Safe_Delete(FileSelector);
	FileSelector = new MY_UI::Controls::File_Selector(MY_UI::Internal::RootWidget);
	FileSelector->SetPos(50, 50);

	FileSelector->OnSelected.Add([this](){
		Static_Mesh* m = new Static_Mesh();
		if(m->Load(FileSelector->GetPathToSelectedFile() + "\\"+ FileSelector->GetSelectedFile())) AddMesh(m);
		else delete m;
	});
	FileSelector->SetSearchFilter(FileExtentions);
}
void Mesh_UI::buttonclicked_animated(){
	MY_UI::Safe_Delete(FileSelector);
	FileSelector = new MY_UI::Controls::File_Selector(MY_UI::Internal::RootWidget);
	FileSelector->SetPos(50, 50);
	FileSelector->OnSelected.Add([this](){
		Animated_Mesh* m = new Animated_Mesh();
		if(m->Load(FileSelector->GetPathToSelectedFile() + "\\"+ FileSelector->GetSelectedFile())) AddMesh(m);
		else delete m;
	});
	FileSelector->SetSearchFilter(FileExtentions);
}
