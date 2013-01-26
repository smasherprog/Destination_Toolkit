#include "PCH.h"
#include "File_Selector.h"
#include "Window.h"
#include "List_Box.h"
#include "Button.h"
#include "Text.h"
#include <sstream>
#include "DropDown.h"

#if defined (WIN32)
	#include "windows.h"
#endif

MY_UI::Controls::File_Selector::File_Selector(cWidgetBase* parent) : cWidgetBase(parent) {
	File_Window = new MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>(this);
	File_Window->SetSize(485, 350);
	File_Window->SetMinSize(475, 340);
	File_Window->SetTitle("File Selector");

	FileList = new MY_UI::Controls::List_Box_wScrollbars(File_Window);
	FileList->SetPos(4, 20);
	FileList->SetSize(300, 300);
	FileList->List->OnSelected.Add(std::bind(&MY_UI::Controls::File_Selector::DisplayFileInfo, this, std::placeholders::_1), this, true);
	FileList->List->SetTextAlignment(LEFT);

	CurrentDir = new MY_UI::Controls::Text(File_Window);
	FileName = new MY_UI::Controls::Text(File_Window);
	CreationTime = new MY_UI::Controls::Text(File_Window);
	LastAccessTime = new MY_UI::Controls::Text(File_Window);
	LastWriteTime = new MY_UI::Controls::Text(File_Window);
	FileSize = new MY_UI::Controls::Text(File_Window);

	MY_UI::Controls::Button* okbutton = new MY_UI::Controls::Button(File_Window);
	okbutton->SetText("Open");
	okbutton->SetPos(310, 270);
	okbutton->OnLeftClicked.Add(std::bind(&MY_UI::Controls::File_Selector::OpenSelectedFile, this));

	MY_UI::Controls::Button* cancelbutton = new MY_UI::Controls::Button(File_Window);
	cancelbutton->SetText("Cancel");
	cancelbutton->Align_To_Widget(okbutton, RIGHT | IN_LINE, 5);
	cancelbutton->OnLeftClicked.Add(std::bind(&MY_UI::Controls::File_Selector::CloseWindow, this));
	
	//DropDownFilter = new MY_UI::Controls::DropDown(File_Window);
	//DropDownFilter->Align_To_Widget(okbutton, BOTTOM | IN_LINE, 5);
	
	char temp[255];
#if defined (WIN32)
	GetCurrentDirectoryA(255, temp);
#endif
	CurrentDirectory = temp;
	SetSearchFilter("*.*");// display all files!
}
MY_UI::Controls::File_Selector::~File_Selector() {	
	for(size_t i =0; i< Files.size(); i ++){
		delete Files[i];// DELETE
	}
	Files.clear();
} 
void MY_UI::Controls::File_Selector::SetSize(int x, int y){ 
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	File_Window->SetSize(x, y);
}
void MY_UI::Controls::File_Selector::SetMinSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetMinSize(x, y);
	File_Window->SetMinSize(x, y);
} 
#if defined (WIN32)
std::string ConvertFileTime(FILETIME * s){

	SYSTEMTIME syst;
	FileTimeToSystemTime(s, &syst);
	std::ostringstream oks;
	oks<<"Date: "<<syst.wMonth<<"/"<<syst.wDayOfWeek<<"/"<<syst.wYear<<std::endl<<"Time: "<<syst.wHour<<":"<<syst.wMinute<<":"<<syst.wSecond;
	std::string st = oks.str();
	return st;
}
#endif

void MY_UI::Controls::File_Selector::SetSearchFilter(std::string filter){// takes a string with a semi colon as a delimiter for example   a string  "*.txt;*her.exe;*.rar"
	Filters.clear();
	SearchFilter = filter;
	std::string currentsearch;
	for(size_t i =0; i< filter.size(); i++){
		if(filter[i] == '*') continue;// skip the asterisk
		if(filter[i] == ';'){// add the string to the search filters
			Filters.push_back(currentsearch);
			currentsearch.clear();// clear the string
		} else {
			currentsearch +=filter[i];// add onto the search pattern
		}
	}
	if(currentsearch.size() != 0) Filters.push_back(currentsearch); // add the last search filter if there is one
	FillListing();
}
void MY_UI::Controls::File_Selector::CloseWindow(){
	MY_UI::Internal::RootWidget->DelayedDelete(this);// delete this after the next loop
}
void MY_UI::Controls::File_Selector::FillListing(){
	SelectedFile="";// clear this each time
	// make sure to clear the array first
	for(size_t i =0; i< Files.size(); i ++){
		delete Files[i];// DELETE
	}
	Files.clear();
	FileList->List->EraseAll();
#if defined (WIN32)

	WIN32_FIND_DATAA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError=0;
	std::string tempdir = CurrentDirectory + "\\*";
	hFind = FindFirstFileA(tempdir.c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		OUTPUT_DEBUG_MSG("Error getting first file in MY_UI::Controls::File_Selector::FillListing()");
		return;
	}
	// List all the files in the directory
	// store the index'
	std::vector<size_t> files;
	std::vector<size_t> dirs;
	do{
		std::string fname = ffd.cFileName;
		if(fname == "." || fname == "..") continue;// skip these
		File_Info *f = new File_Info;
		f->CreationTime = "    Created: \n";
		f->CreationTime += ConvertFileTime(&ffd.ftCreationTime);
		f->LastAccessTime = "    Last Accessed: \n";
		f->LastAccessTime += ConvertFileTime(&ffd.ftLastAccessTime);
		f->LastWriteTime = "    Last Written To: \n";
		f->LastWriteTime += ConvertFileTime(&ffd.ftLastWriteTime);
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){		
			f->FileSize = "0";// no Size!
			f->IsFile = false;
			f->FileName = "<DIR> ";
			Files.push_back(f);
			dirs.push_back(Files.size()-1);
		} else {
			if(!MatchesFilter(fname)){
				delete f;// free the memory
				continue;
			}
			std::ostringstream oks;
			oks<<ffd.nFileSizeLow;
			f->FileSize = "    File Size (in bytes): \n";
			f->FileSize += oks.str();
			f->IsFile = true;
			f->FileName = "<FILE> ";
			Files.push_back(f);
			files.push_back(Files.size()-1);
		}	
		
		f->FileName += fname;
	}

	while (FindNextFileA(hFind, &ffd) != 0);

	if(CurrentDirectory.size() > 3) {// we are not to the root of the drive yet... add a directry up button  the sise of CurrentDirectory will be 3 when we are at the root, i.e.  "  c:\     "
		File_Info *f = new File_Info;
		f->CreationTime = "    Created: \n";
		f->LastAccessTime = "    Last Accessed: \n";
		f->LastWriteTime = "    Last Written To: \n";
		f->FileSize = "    File Size (in bytes): \n";
		f->FileName = "Go Up a Directory";
		f->IsFile = false;
		Files.push_back(f);
		MY_UI::Controls::Button* b = FileList->Insert(f->FileName);
		b->Internal_Widget.ExtraData = Files[Files.size()-1];// this is just holding the pointer, 
		b->OnLeftDoubleClick.Add(std::bind(&MY_UI::Controls::File_Selector::OpenFile, this, std::placeholders::_1), this, true);
	}
	
	for(size_t i =0; i< dirs.size(); i++){
		MY_UI::Controls::Button* b = FileList->Insert(Files[dirs[i]]->FileName);
		b->Internal_Widget.ExtraData = Files[dirs[i]];// this is just holding the pointer, 
		b->OnLeftDoubleClick.Add(std::bind(&MY_UI::Controls::File_Selector::OpenFile, this, std::placeholders::_1), this, true);
	}
	for(size_t i =0; i< files.size(); i++){
		MY_UI::Controls::Button* b = FileList->Insert(Files[files[i]]->FileName);
		b->Internal_Widget.ExtraData = Files[files[i]];// this is just holding the pointer, 
		b->OnLeftDoubleClick.Add(std::bind(&MY_UI::Controls::File_Selector::OpenFile, this, std::placeholders::_1), this, true);
	}
			
#endif
	CurrentDir->SetText(CurrentDirectory);
	CurrentDir->SetPos(2, 2);
	CurrentDir->SetSizeToContents();
}
bool MY_UI::Controls::File_Selector::MatchesFilter(const std::string& str){
	if(SearchFilter == "*.*") return true;// select all files!
	for(size_t i(0); i< Filters.size(); i++){
		auto found = str.find(Filters[i]);
		if(found != std::string::npos){// Match found!
			return true;
		}
	}
	return false;// no matches found for this file...
}
void MY_UI::Controls::File_Selector::OpenSelectedFile(){
	MY_UI::Controls::Button* b = FileList->List->GetSelected();
	if(b == 0) return;// nothing to see here
	OpenFile(b);
}
void MY_UI::Controls::File_Selector::OpenFile(cWidgetBase* selected){
	MY_UI::Controls::Button* b = static_cast<MY_UI::Controls::Button*>(selected);
	File_Info* finfo = (File_Info*)b->Internal_Widget.ExtraData;
	
	std::string temp1=finfo->FileName;
	if(finfo->IsFile){
		temp1.erase(0, 7);//erase the text I added
		SelectedFile = temp1;
		OnSelected.Call(this);// run the callback and close the window
		CloseWindow();
	} else {// otherwise, change directories
		if(temp1 == "Go Up a Directory"){
			auto found = CurrentDirectory.find_last_of("\\");
			if(found != std::string::npos){
				CurrentDirectory.erase(found, CurrentDirectory.size() - found);
			}
		} else {
			temp1.erase(0, 6);//erase the text I added
			CurrentDirectory+="\\" + temp1;
		}
		FillListing();
	}

}
void MY_UI::Controls::File_Selector::DisplayFileInfo(MY_UI::Controls::cWidgetBase* selected){
	MY_UI::Controls::List* b = static_cast<MY_UI::Controls::List*>(selected);
	MY_UI::Controls::Button* t = b->GetSelected();
	File_Info* finfo = (File_Info*)t->Internal_Widget.ExtraData;
	
	std::string temp;
	std::string temp1=finfo->FileName;
	if(finfo->IsFile){
		temp = "    File name:\n";
		temp1.erase(0, 7);
	} else {
		temp = "    Directory name:\n";
		temp1.erase(0, 6);
	}
	temp +=temp1;

	FileName->SetText(temp);
	FileName->SetSizeToContents();
	FileName->Align_To_Widget(FileList, RIGHT, 10);
	FileName->SetPos(FileName->GetPos().x, 12);// move it down 12 units because directory names can be vewwy long 
	
	CreationTime->SetText(finfo->CreationTime);
	CreationTime->SetSizeToContents();
	CreationTime->Align_To_Widget(FileName, BOTTOM | IN_LINE, 5);

	LastAccessTime->SetText(finfo->LastAccessTime);
	LastAccessTime->SetSizeToContents();
	LastAccessTime->Align_To_Widget(CreationTime, BOTTOM | IN_LINE, 5);

	LastWriteTime->SetText(finfo->LastWriteTime);
	LastWriteTime->SetSizeToContents();
	LastWriteTime->Align_To_Widget(LastAccessTime, BOTTOM | IN_LINE, 5);
	if(finfo->IsFile){
		FileSize->SetText(finfo->FileSize);
		FileSize->SetSizeToContents();
		FileSize->Align_To_Widget(LastWriteTime, BOTTOM | IN_LINE, 5);
	} else {
		FileSize->SetText("");
	}
}