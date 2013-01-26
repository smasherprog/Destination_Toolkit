#include "stdafx.h"
#include "cCharacterSelection.h"
#include "../MY_UI\List_Box.h"
#include "../MY_UI\Button.h"
#include "../MY_UI\TextBox.h"
#include "../MY_UI\Text.h"
#include "../MY_UI\Button.h"
#include "Global_GameData.h"
#include "cDemoControl.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"

cCharacterSelection* cCharacterSelection::self=0;

cCharacterSelection::cCharacterSelection(cWidgetBase* parent, cDemoControl* demo) : MY_UI::Controls::Window(parent), DemoControl(*demo) {
	assert(self==0);
	SetSize(400, 300);
	SetPos(300, 300);
	SetCloseable(false);// do not allow a user to close this
	SetTitle("Character Selection");

	CharListing = new MY_UI::Controls::List_Box( this );
	CharListing->SetPos(10, 10);
	CharListing->SetSize(120, 200 );

	MY_UI::Controls::Button* btn = new MY_UI::Controls::Button( this );
	btn->SetText("Enter World");
	btn->Align_To_Widget(CharListing, CENTER |BOTTOM, 10);
	btn->OnLeftClicked.Add(std::bind(&cCharacterSelection::EnterWorld, this));

	MY_UI::Controls::Text* label = new MY_UI::Controls::Text( this );
	label->SetText( "Create New Character" );
	label->SetSizeToContents();
	label->SetPos( 140, 10 );

	CharacterCreation = new MY_UI::Controls::TextBox( this );
	CharacterCreation->SetMaxCharacters(11);
	CharacterCreation->SetLength(140);
	CharacterCreation->Align_To_Widget(label, BOTTOM | IN_LINE, 5);

	btn = new MY_UI::Controls::Button( this );
	btn->SetText("Create");
	btn->Align_To_Widget(CharacterCreation, BOTTOM | IN_LINE, 5);
	btn->OnLeftClicked.Add(std::bind(&cCharacterSelection::CreateCharacter, this));

	Issues = new MY_UI::Controls::Text( this );
	Issues->SetText( "" );
	Issues->Align_To_Widget(btn, BOTTOM | IN_LINE, 5);
}
cCharacterSelection::~cCharacterSelection() {
	self=0;
}
void cCharacterSelection::Init(char* data, unsigned int length, size_t channel){
	unsigned char e = *data++;
	unsigned int end = static_cast<unsigned int>(e);

	for(unsigned int i(0); i< end; i++){
		Characters[i] = std::make_shared<cCharacter>();
		Characters[i]->ConstructForVisual(data);	
		AddCharacter(Characters[i]->Name);
	}
}
void  cCharacterSelection::AddCharacter(std::string name){
	CharListing->Insert(name);
}
void cCharacterSelection::EnterWorld(){
	MY_UI::Controls::Button* b = CharListing->GetSelected();
	if(b == 0){
		Issues->SetText("You must select a character");
		Issues->SetSizeToContents();
		Issues->SetColor(MY_UI::Utilities::Color( 255, 255, 0, 0 ) );
		return;
	}
	ActiveCharacter=255;
	for(size_t i(0); i< MAXCHARACTERS; i++){
		if(Characters[i]->Created()){
			if(Characters[i]->Name == b->GetText()){
				ActiveCharacter = (unsigned char)i;
				break;
			}
		}
	}
	if(ActiveCharacter==255) {
		OUTPUT_DEBUG_MSG("Could not find character to enter world with.. ughhh");
		return;
	}
	cDataPacket pack(1);
	char* beg = pack.Data;
	*reinterpret_cast<uint16_t*>(beg) = Network::GS_LOGINTO_WORLD;
	beg+=2;
	*beg = ActiveCharacter;
	DemoControl.ReliableSendTo(pack);

}	

void cCharacterSelection::CreateCharacter(){
	std::string name = CharacterCreation->GetText();
	if(name.empty()) {
		Issues->SetText("You must enter a name");
		Issues->SetSizeToContents();
		Issues->SetColor(MY_UI::Utilities::Color( 255, 255, 0, 0 ) );
		return;
	}
	cDataPacket pack(NAMESIZE +1);
	char* beg = pack.Data;
	pack.SetID(Network::GS_CREATE_CHARACTER);

	*beg++=static_cast<char>(name.size());
	OUTPUT_DEBUG_MSG("Sending request to create a character, named "<<name);
	memcpy(beg, name.c_str(), name.size());
	DemoControl.ReliableSendTo(pack);
}
