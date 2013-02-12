#include "stdafx.h"
#include "Mesh_UI.h"
#include "../MY_UI/Window.h"
#include "../MY_UI/Button.h"
#include "../MY_UI/File_Selector.h"

Mesh_UI::Mesh_UI(){
	FileExtentions = "*.*";// all files

	MY_UI::Controls::Button* Button = new MY_UI::Controls::Button(MY_UI::Internal::RootWidget);
	Button->SetPos(10, 60);
	Button->SetText("Load Mesh", CENTER);
	Button->SetPopup("Click here to load a static mesh");
	//capture the button by copy and the class as a reference
	Button->OnLeftClicked.Add(std::bind(&Mesh_UI::buttonclicked, this));
}
Mesh_UI::~Mesh_UI(){
	MY_UI::Safe_Delete(FileSelector);
}
void Mesh_UI::buttonclicked(){
	MY_UI::Safe_Delete(FileSelector);
	FileSelector = new MY_UI::Controls::File_Selector(MY_UI::Internal::RootWidget);
	FileSelector->SetPos(50, 50);
	FileSelector->OnSelected.Add([this](){
		AddMesh(FileSelector->GetPathToSelectedFile() + "\\"+ FileSelector->GetSelectedFile());
	});
	FileSelector->SetSearchFilter(FileExtentions);
}
