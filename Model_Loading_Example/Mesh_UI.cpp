#include "stdafx.h"
#include "Mesh_UI.h"
#include "../MY_UI/Window.h"
#include "../MY_UI/Button.h"
#include "../MY_UI/File_Selector.h"

Mesh_UI::Mesh_UI(){
	FileExtentions = "*.*";// all files
	window = new MY_UI::Controls::Window(MY_UI::Internal::RootWidget);
	window->SetPos(300, 150);
	window->SetSize(100, 200);
	window->SetTitle("Mesh UI");
	window->SetCloseable(false);// dont allow the window to close

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(window);
	pbutton->SetPos(10, 10);
	pbutton->SetText("Files", CENTER);
	pbutton->SetPopup("Create file Selector");
	// the lambda takes copies of the pointers, but this is safe since each of these are children and they cannot exist without their parents.
	pbutton->OnLeftClicked.Add([=](){
		MY_UI::Controls::File_Selector* f = new MY_UI::Controls::File_Selector(window);
		f->SetPos(50, 50);
		f->OnSelected.Add([=](){
			AddMesh(f->GetPathToSelectedFile() + "\\"+ f->GetSelectedFile());
		});
		f->SetSearchFilter(FileExtentions);
	});
}
