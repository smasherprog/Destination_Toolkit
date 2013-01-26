#ifndef CFILE_SELECTOR_H
#define CFILE_SELECTOR_H
#include "cWidgetBase.h"
#include <string>
#include "Resizable.h"

namespace MY_UI{

	namespace Controls{

		class List_Box_wScrollbars;
		class DropDown;
		class File_Selector: public cWidgetBase {
		public:

			File_Selector(cWidgetBase* parent);
			virtual ~File_Selector();

			virtual void SetSize(int x, int y);
			virtual void SetMinSize(int x, int y);

			virtual void DisplayFileInfo(cWidgetBase* selected);
			virtual void SetSearchFilter(std::string filter);
			virtual void OpenFile(cWidgetBase* selected);
			virtual void OpenSelectedFile();
			virtual bool MatchesFilter(const std::string& str);

			virtual void FillListing();
			virtual std::string GetSelectedFile(){ return SelectedFile; }// this is the selected file, i.e.  see.txt
			virtual std::string GetPathToSelectedFile(){ return CurrentDirectory; }// this is the path to the selected file, i.e.  c:\blarf\seemee

			virtual void CloseWindow();

			std::string CurrentDirectory, SearchFilter, SelectedFile;
			std::vector<std::string> Filters;// containes each search filter

			MY_UI::Controls::List_Box_wScrollbars* FileList;
			MY_UI::Controls::Text* FileName, *CreationTime, *LastAccessTime, *LastWriteTime, *FileSize, *CurrentDir;
			MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>* File_Window;
			MY_UI::Controls::DropDown* DropDownFilter;

			MY_UI::Internal::cWidgetCaller OnSelected;// when a file is selected, the window will close after the call to OnSelected

			struct File_Info{
				int IsFile;// if IsFile is false, the file is a directory
				std::string FileName, CreationTime, LastAccessTime, LastWriteTime, FileSize;
			};
			std::vector<File_Info*> Files;// ALLOCATED MEMORY IN HERE!!!!!
		};
	};
};

#endif