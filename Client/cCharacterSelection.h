#ifndef CCHARACTER_SELECTION_H
#define CCHARACTER_SELECTION_H

#include "../MY_UI\Window.h"
//forward delcarations
class cDemoControl;
namespace MY_UI{
	namespace Controls {
		
		class TextBox;
		class Text;
		class List_Box;
	};
};


class cCharacterSelection : public MY_UI::Controls::Window{
public:

	static cCharacterSelection* self;

	cCharacterSelection(cWidgetBase* parent, cDemoControl* demo);
	virtual ~cCharacterSelection() ;
	cDemoControl& DemoControl;

	void AddCharacter(std::string name);
	void EnterWorld();
	void Init(char* data, unsigned int length, size_t channel);
	void CreateCharacter();
		
	MY_UI::Controls::TextBox* CharacterCreation;
	MY_UI::Controls::Text* Issues;
	MY_UI::Controls::List_Box* CharListing;

};

#endif