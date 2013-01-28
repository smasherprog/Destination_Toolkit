#ifndef MESH_UI
#define MESH_UI
#include <functional>

namespace MY_UI{
	namespace Controls{
		class Window;
	}
}

class Mesh_UI{
public:

	Mesh_UI();
	void Set_AddMesh_Func(std::function<bool (std::string file)> f){ AddMesh = f;}
	//takes a string with a semi colon as a delimeter, for example   "*.exe;*.jpg;"
	void SetFileExts(std::string s){ FileExtentions =s; }

protected:
	
	std::string FileExtentions;
	std::function<bool (std::string file)> AddMesh;
	MY_UI::Controls::Window* window;// ui window
};


#endif