#include "PCH.h"
#include "ColorPickerWindow.h"
#include "cWidgetSkin.h"
#include "Button.h"
#include "Image.h"
#include "Text.h"

///////////////////////////////////////////////////////////  ColorPicker

MY_UI::Controls::ColorPicker::ColorPicker(cWidgetBase* parent): cWidgetBase(parent), ColorPickerWindow(0) { 
	Users_Color=0;
	Users_Red=Users_Green=Users_Blue=Users_Alpha=0;
	SetControlBounds(Utilities::Rect(0, 0, 30, 30));
	SetClientBounds(Utilities::Rect(0, 0, 30, 30));
	ColorPalet = new MY_UI::Controls::ColoredRect(this);
	ColorPalet->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPicker::OpenColorWindow, this));
	ColorPalet->SetColor(MY_UI::Utilities::Red);
	SetSize(30, 30);
	SetPos(0, 0);
}

void MY_UI::Controls::ColorPicker::OpenColorWindow(){
	if(ColorPickerWindow != 0) delete ColorPickerWindow;
	ColorPickerWindow = new MY_UI::Controls::ColorPickerWindow(this);
	ColorPickerWindow->OnDestroyed.Add(std::bind(&MY_UI::Controls::ColorPicker::ColorPickerDestroyed, this));
	ColorPickerWindow->Internal_ColorPicker.Apply->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPicker::SetColor, this));
	ColorPickerWindow->SetPos(30, 30);
	ColorPickerWindow->SetPickedColor(ColorPalet->GetColor());
}
void MY_UI::Controls::ColorPicker::ColorPickerDestroyed(){
	ColorPickerWindow =0;
}
void MY_UI::Controls::ColorPicker::SetLink(Utilities::Color* color) { 
	Users_Color = color; 
	ColorPalet->SetColor(*color);
}

void MY_UI::Controls::ColorPicker::SetLink(float* red, float* green, float* blue, float* alpha) {
	Users_Red =red; 
	Users_Green =green;  
	Users_Blue =blue;  
	Users_Alpha =alpha;  
	MY_UI::Utilities::Color c;
	if(Users_Red !=0 ) c.Red = static_cast<unsigned char>(*red)*255;// float colors are always in the range [0, 1]
	if(Users_Green !=0 ) c.Green  = static_cast<unsigned char>(*green)*255;// float colors are always in the range [0, 1]
	if(Users_Blue !=0 ) c.Blue =static_cast<unsigned char>(*blue)*255;// float colors are always in the range [0, 1]
	if(Users_Alpha !=0 ) c.Alpha = static_cast<unsigned char>(*alpha)*255;// float colors are always in the range [0, 1]
	ColorPalet->SetColor(c);
}
	
void MY_UI::Controls::ColorPicker::SetSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Utilities::Point size = GetSize();
	ColorPalet->SetSize(size.x, size.y);
}
// this is called when the apply button is pressed in the color picker window
void MY_UI::Controls::ColorPicker::SetColor(){
	ColorPalet->SetColor(ColorPickerWindow->GetColor());
	MY_UI::Utilities::Color c = ColorPalet->GetColor();
	if(Users_Color != 0) *Users_Color= c;
	if(Users_Red !=0 ) *Users_Red = static_cast<float>(c.Red)/255.0f;// float colors are always in the range [0, 1]
	if(Users_Green !=0 ) *Users_Green = static_cast<float>(c.Green)/255.0f;// float colors are always in the range [0, 1]
	if(Users_Blue !=0 ) *Users_Blue = static_cast<float>(c.Blue)/255.0f;// float colors are always in the range [0, 1]
	if(Users_Alpha !=0 ) *Users_Alpha = static_cast<float>(c.Alpha)/255.0f;// float colors are always in the range [0, 1]
	OnColorSelected.Call(this);

}

///////////////////////////////////////////////////////////  ColorPickerWindow



MY_UI::Controls::ColorPickerWindow::ColorPickerWindow(cWidgetBase* parent): cWidgetBase(parent) { 
	// manually set the starting size of the window
	SetControlBounds(Utilities::Rect(0, 0, 350, 220));

	SetClientBounds(Utilities::Rect(8, 8, 342, 212));

	auto skin = MY_UI::Internal::WidgetSkin;
	// setting the controls to NULL means that they will not be inserted in the children array
	Internal_ColorPicker.Background = new Image(this);
	Internal_ColorPicker.Background->SetPos(-8, -8);
	Internal_ColorPicker.Background->Set_Texture(skin->Window_Without_Focus);// take a copy of the UI skin for the texture
	Internal_ColorPicker.Background->SetName("Background");
	Internal_ColorPicker.Background->SetDraggable(true);
	OnLeftDrag.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::Dragging, this));
	DeCouple(Internal_ColorPicker.Background);

	Internal_ColorPicker.Circle = new Controls::Button(this);
	Internal_ColorPicker.Circle->Set_Texture(skin->White_Circle);
	Internal_ColorPicker.Circle->SetDraggable(true);
	Internal_ColorPicker.Circle->SetSize(static_cast<int>(skin->White_Circle.width ), static_cast<int>(skin->White_Circle.height));
	Internal_ColorPicker.Circle->SetPos(150, 50);
	Internal_ColorPicker.Circle->OnLeftDrag.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::DraggingCircle, this));
	Internal_ColorPicker.Circle->SetName("circle");

	Internal_ColorPicker.ColorArea = new MY_UI::Controls::ColoredRect(this);
	Internal_ColorPicker.ColorArea->SetSize(196, 204);
	Internal_ColorPicker.ColorArea->SetColors(MY_UI::Utilities::White, MY_UI::Utilities::Red, MY_UI::Utilities::Black, MY_UI::Utilities::Black);
	Internal_ColorPicker.ColorArea->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::MoveCircle,this));

	Internal_ColorPicker.ColorStrip_0 = new MY_UI::Controls::ColoredRect(this);
	Internal_ColorPicker.ColorStrip_0->SetSize(30, Internal_ColorPicker.ColorArea->GetSize().y/6);
	Internal_ColorPicker.ColorStrip_0->Align_To_Widget(Internal_ColorPicker.ColorArea, RIGHT | IN_LINE, 16);
	Internal_ColorPicker.ColorStrip_0->SetColors(MY_UI::Utilities::Red, MY_UI::Utilities::Red, MY_UI::Utilities::Red | MY_UI::Utilities::Blue, MY_UI::Utilities::Red | MY_UI::Utilities::Blue);
	Internal_ColorPicker.ColorStrip_0->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::MoveToStrip, this));

	Internal_ColorPicker.RightArrow = new Controls::Button(this);
	Internal_ColorPicker.RightArrow->SetDraggable(true);
	Internal_ColorPicker.RightArrow->Set_Texture(skin->Right_Slider_Arrow);
	Internal_ColorPicker.RightArrow->SetSize(static_cast<int>(skin->Right_Slider_Arrow.width ), static_cast<int>(skin->Right_Slider_Arrow.height));
	Internal_ColorPicker.RightArrow->Align_To_Widget(Internal_ColorPicker.ColorStrip_0, LEFT | IN_LINE, 3);
	Internal_ColorPicker.RightArrow->OnLeftDrag.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::DraggingArrow, this));

	Internal_ColorPicker.LeftArrow = new Controls::Button(this);
	Internal_ColorPicker.LeftArrow->SetDraggable(true);
	Internal_ColorPicker.LeftArrow->Set_Texture(skin->Left_Slider_Arrow);
	Internal_ColorPicker.LeftArrow->SetSize(static_cast<int>(skin->Left_Slider_Arrow.width ), static_cast<int>(skin->Left_Slider_Arrow.height));
	Internal_ColorPicker.LeftArrow->Align_To_Widget(Internal_ColorPicker.ColorStrip_0, RIGHT | IN_LINE, 3);
	Internal_ColorPicker.LeftArrow->OnLeftDrag.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::DraggingArrow, this));

	Internal_ColorPicker.ColorStrip_1 = new MY_UI::Controls::ColoredRect(this);
	Internal_ColorPicker.ColorStrip_1->SetSize(Internal_ColorPicker.ColorStrip_0->GetSize());
	Internal_ColorPicker.ColorStrip_1->Align_To_Widget(Internal_ColorPicker.ColorStrip_0, BOTTOM | IN_LINE, 0);
	Internal_ColorPicker.ColorStrip_1->SetColors(MY_UI::Utilities::Red | MY_UI::Utilities::Blue, MY_UI::Utilities::Red | MY_UI::Utilities::Blue, MY_UI::Utilities::Blue, MY_UI::Utilities::Blue);	
	Internal_ColorPicker.ColorStrip_1->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::MoveToStrip,this));

	Internal_ColorPicker.ColorStrip_2 = new MY_UI::Controls::ColoredRect(this);
	Internal_ColorPicker.ColorStrip_2->SetSize(Internal_ColorPicker.ColorStrip_1->GetSize());
	Internal_ColorPicker.ColorStrip_2->Align_To_Widget(Internal_ColorPicker.ColorStrip_1, BOTTOM | IN_LINE, 0);
	Internal_ColorPicker.ColorStrip_2->SetColors(MY_UI::Utilities::Blue ,  MY_UI::Utilities::Blue,  MY_UI::Utilities::Blue | MY_UI::Utilities::Green, MY_UI::Utilities::Blue | MY_UI::Utilities::Green);
	Internal_ColorPicker.ColorStrip_2->OnLeftClicked.Add(std::bind( &MY_UI::Controls::ColorPickerWindow::MoveToStrip, this));

	Internal_ColorPicker.ColorStrip_3 = new MY_UI::Controls::ColoredRect(this);
	Internal_ColorPicker.ColorStrip_3->SetSize(Internal_ColorPicker.ColorStrip_2->GetSize());
	Internal_ColorPicker.ColorStrip_3->Align_To_Widget(Internal_ColorPicker.ColorStrip_2, BOTTOM | IN_LINE, 0);
	Internal_ColorPicker.ColorStrip_3->SetColors(MY_UI::Utilities::Blue | MY_UI::Utilities::Green, MY_UI::Utilities::Blue | MY_UI::Utilities::Green, MY_UI::Utilities::Green, MY_UI::Utilities::Green);
	Internal_ColorPicker.ColorStrip_3->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::MoveToStrip, this));
		
	Internal_ColorPicker.ColorStrip_4 = new MY_UI::Controls::ColoredRect(this);
	Internal_ColorPicker.ColorStrip_4->SetSize(Internal_ColorPicker.ColorStrip_3->GetSize());
	Internal_ColorPicker.ColorStrip_4->Align_To_Widget(Internal_ColorPicker.ColorStrip_3, BOTTOM | IN_LINE, 0);
	Internal_ColorPicker.ColorStrip_4->SetColors(MY_UI::Utilities::Green , MY_UI::Utilities::Green, MY_UI::Utilities::Green |  MY_UI::Utilities::Red, MY_UI::Utilities::Green |  MY_UI::Utilities::Red);
	Internal_ColorPicker.ColorStrip_4->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::MoveToStrip, this));
	
	Internal_ColorPicker.ColorStrip_5 = new MY_UI::Controls::ColoredRect(this);
	Internal_ColorPicker.ColorStrip_5->SetSize(Internal_ColorPicker.ColorStrip_4->GetSize());
	Internal_ColorPicker.ColorStrip_5->Align_To_Widget(Internal_ColorPicker.ColorStrip_4, BOTTOM | IN_LINE, 0);
	Internal_ColorPicker.ColorStrip_5->SetColors(MY_UI::Utilities::Green |  MY_UI::Utilities::Red , MY_UI::Utilities::Green | MY_UI::Utilities::Red, MY_UI::Utilities::Red, MY_UI::Utilities::Red);
	Internal_ColorPicker.ColorStrip_5->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::MoveToStrip, this));

	RedBox = new MY_UI::Controls::cWidget_w_Label<Numeric_TextBox<unsigned int>>(this);
	RedBox->Widget->SetLength(30);
	RedBox->Widget->SetMaxCharacters(3);
	RedBox->SetLabel("R:", LEFT | IN_LINE, 5);
	RedBox->Align_To_Widget(Internal_ColorPicker.ColorArea, RIGHT | IN_LINE, 60);
	RedBox->Widget->SetMin(0);
	RedBox->Widget->SetMax(255);

	GreenBox = new MY_UI::Controls::cWidget_w_Label<Numeric_TextBox<unsigned int>>(this);
	GreenBox ->Widget->SetLength(30);
	GreenBox->Widget->SetMaxCharacters(3);
	GreenBox ->SetLabel("G:", LEFT | IN_LINE, 5);
	GreenBox ->Align_To_Widget(RedBox, BOTTOM | IN_LINE, 4);
	GreenBox->Widget->SetMin(0);
	GreenBox->Widget->SetMax(255);

	BlueBox = new MY_UI::Controls::cWidget_w_Label<Numeric_TextBox<unsigned int>>(this);
	BlueBox->Widget->SetLength(30);
	BlueBox->Widget->SetMaxCharacters(3);
	BlueBox->SetLabel("B:", LEFT | IN_LINE, 5);
	BlueBox->Align_To_Widget(GreenBox, BOTTOM | IN_LINE, 4);
	BlueBox->Widget->SetMin(0);
	BlueBox->Widget->SetMax(255);

	Internal_ColorPicker.PickedColor = new MY_UI::Controls::ColoredRect(this);
	Internal_ColorPicker.PickedColor->SetSize(70, 70);
	Internal_ColorPicker.PickedColor->Align_To_Widget(BlueBox, BOTTOM | IN_LINE, 4);
	Internal_ColorPicker.PickedColor->SetColor(MY_UI::Utilities::Red);

	Internal_ColorPicker.Apply = new MY_UI::Controls::Button(this);
	Internal_ColorPicker.Apply->SetLength(50);
	Internal_ColorPicker.Apply->SetText("Apply");
	Internal_ColorPicker.Apply->Align_To_Widget(Internal_ColorPicker.PickedColor, BOTTOM | IN_LINE, 4);
	Internal_ColorPicker.Apply->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::Close, this));

	Internal_ColorPicker.Cancel = new MY_UI::Controls::Button(this);
	Internal_ColorPicker.Cancel->SetLength(50);
	Internal_ColorPicker.Cancel->SetText("Cancel");
	Internal_ColorPicker.Cancel->Align_To_Widget(Internal_ColorPicker.Apply, BOTTOM | IN_LINE, 4);
	Internal_ColorPicker.Cancel->OnLeftClicked.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::Close, this));

	RedBox->Widget->SetLink(&Internal_ColorPicker.Red);
	RedBox->Widget->OnKeyUp.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::ReverseColorApply, this));
	BlueBox->Widget->SetLink(&Internal_ColorPicker.Blue);
	BlueBox->Widget->OnKeyUp.Add(std::bind(&MY_UI::Controls::ColorPickerWindow::ReverseColorApply, this));
	GreenBox->Widget->SetLink(&Internal_ColorPicker.Green);
	GreenBox->Widget->OnKeyUp.Add(std::bind( &MY_UI::Controls::ColorPickerWindow::ReverseColorApply, this));

	SetSize(350, 220);
}
MY_UI::Controls::ColorPickerWindow::~ColorPickerWindow(){
	delete Internal_ColorPicker.Background;
}
void MY_UI::Controls::ColorPickerWindow::Close(){
	SetColor(Internal_ColorPicker.PickedColor->GetColor());
	DeCouple(this);// unattach this widget from a parent if there is any
	MY_UI::Internal::RootWidget->DelayedDelete(this);// add this to a delayed delete in case this close is called internally
	//Delaying the deletion of components is very important because it could be done by the user, or internally so I want the object to
	//exist through the entire loop in case any components need it
}

void MY_UI::Controls::ColorPickerWindow::SetFocus(bool focus){
	cWidgetBase::SetFocus(focus);
	auto skin = MY_UI::Internal::WidgetSkin;
	if(focus) Internal_ColorPicker.Background->Set_Texture(skin->Window_With_Focus);// take a copy of the UI skin for the texture
	 else  Internal_ColorPicker.Background->Set_Texture(skin->Window_Without_Focus);// take a copy of the UI skin for the texture
}
void MY_UI::Controls::ColorPickerWindow::SetPos(int x, int y){
	Internal_Widget.Children.push_back(Internal_ColorPicker.Background);
	MY_UI::Controls::cWidgetBase::SetPos(x, y);
	Internal_Widget.Children.pop_back();
}
void MY_UI::Controls::ColorPickerWindow::SetSize(int x, int y){ 
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Utilities::Point size = GetSize();
	Internal_ColorPicker.Background->SetSize(size.x, size.y);
	MY_UI::Controls::cWidgetBase::Update();
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::ColorPickerWindow::Hit(){
	if(Internal_Widget.Hidden) return 0;
	if(Internal_ColorPicker.Background->Hit()){
		MY_UI::Controls::cWidgetBase* controlhit = MY_UI::Controls::cWidgetBase::Hit();
		if(controlhit !=0 ) return controlhit;
		else return this;
	}
	return 0;
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::ColorPickerWindow::Hit_And_SetFocus(){
	if(Internal_Widget.Hidden) return 0;
	if(Internal_ColorPicker.Background->Hit()){
		if(Internal_ColorPicker.ColorArea->Hit()) return Internal_ColorPicker.Circle;
		MY_UI::Controls::cWidgetBase* controlhit = MY_UI::Controls::cWidgetBase::Hit_And_SetFocus();
		if(controlhit !=0 ) return controlhit;
		else return this;
	}
	return 0;
}
	
void MY_UI::Controls::ColorPickerWindow::Draw_Clip(){
	Draw_NoClip();// never clip the color picker window!!
}
void MY_UI::Controls::ColorPickerWindow::Draw_NoClip(){
	if(Internal_Widget.Hidden) return;
	Internal_ColorPicker.Background->Draw_NoClip();
	for(size_t i(0); i < Internal_Widget.Children.size() ; i++) {
		Internal_Widget.Children[i]->Draw_NoClip(); 
	}
	Internal_ColorPicker.Circle->Draw_NoClip();
}
void MY_UI::Controls::ColorPickerWindow::Update(){
	Utilities::Point centerpos = Internal_ColorPicker.Circle->GetPos();
	Utilities::Point size = Internal_ColorPicker.Circle->GetSize();
	size /=2;
	centerpos += size;// this is now the real center of the circle
	
	Utilities::Point controlsize = Internal_ColorPicker.ColorArea->GetSize();
	float percenttowhite = 1.0f -  (static_cast<float>(centerpos.x) / static_cast<float>(controlsize.x));
	
	float percenttoblack = 1.0f - (static_cast<float>(centerpos.y) / static_cast<float>(controlsize.y));
	Utilities::Color col = Internal_ColorPicker.ColorArea->Internal_Widget.Color_Top_Right;// starting color
	col = Utilities::Lerp(col, MY_UI::Utilities::White, percenttowhite);
	col *= percenttoblack;
	col.Alpha = 255;
	Internal_ColorPicker.PickedColor->SetColor(col);
	Internal_ColorPicker.Red = col.Red;
	Internal_ColorPicker.Blue = col.Blue;
	Internal_ColorPicker.Green = col.Green;
}
/*-----------------------------Added Functions go here -----------------------------------------*/


void MY_UI::Controls::ColorPickerWindow::Dragging(){	
	Utilities::Point p = GetPos();
	p.x += New_MousePosx - Old_MousePosx;
	p.y += New_MousePosy - Old_MousePosy;
	SetPos(p.x, p.y);
}

void MY_UI::Controls::ColorPickerWindow::DraggingArrow(){
	Utilities::Point p = Internal_ColorPicker.RightArrow->GetPos();
	Utilities::Point arrowsize = Internal_ColorPicker.RightArrow->GetSize();
	auto skin = MY_UI::Internal::WidgetSkin;
	int dif = New_MousePosy - Old_MousePosy;
	int ypos = p.y + dif;
	ypos = Utilities::Clamp<int>(ypos, -arrowsize.y/2, Internal_ColorPicker.ColorArea->GetSize().y  - arrowsize.y/2);
	Internal_ColorPicker.RightArrow->SetPos(p.x, ypos);
	p = Internal_ColorPicker.LeftArrow->GetPos();
	Internal_ColorPicker.LeftArrow->SetPos(p.x, ypos);
	SetSliderFromArrows();
	Update();
}
void MY_UI::Controls::ColorPickerWindow::SetSliderFromArrows(){
	Utilities::Point p = Internal_ColorPicker.RightArrow->GetPos();
	Utilities::Point arrowsize = Internal_ColorPicker.RightArrow->GetSize();
	auto skin = MY_UI::Internal::WidgetSkin;
	int ypos = p.y;
	ypos = Utilities::Clamp<int>(ypos, -arrowsize.y/2, Internal_ColorPicker.ColorArea->GetSize().y  - arrowsize.y/2);
	ypos +=(arrowsize.y/2);
	float y = static_cast<float>(ypos);
	Utilities::Point stripsize = Internal_ColorPicker.ColorStrip_0->GetSize();
	float segmentsize = static_cast<float>(stripsize.y);
	if(ypos < stripsize.y){
		float percentofblue = y / segmentsize;
		unsigned char blue= static_cast<unsigned char>(percentofblue * 255.0f);
		Internal_ColorPicker.ColorArea->SetColors(MY_UI::Utilities::White, Utilities::Color(255, 255, 0, blue ), MY_UI::Utilities::Black, MY_UI::Utilities::Black);
	} else if(ypos < stripsize.y *2){
		y-=segmentsize;
		float percentofred= 1.0f - (y / segmentsize);
		unsigned char red = static_cast<unsigned char>(percentofred * 255.0f);
		Internal_ColorPicker.ColorArea->SetColors(MY_UI::Utilities::White, Utilities::Color(255, red, 0, 255 ), MY_UI::Utilities::Black, MY_UI::Utilities::Black);
	} else if(ypos < stripsize.y *3){
		y-=segmentsize*2.0f;
		float percentofgreen = y / segmentsize;
		unsigned char green = static_cast<unsigned char>(percentofgreen * 255.0f);
		Internal_ColorPicker.ColorArea->SetColors(MY_UI::Utilities::White, Utilities::Color(255, 0, green, 255 ), MY_UI::Utilities::Black, MY_UI::Utilities::Black);
	} else if(ypos < stripsize.y *4){
		y-=segmentsize*3.0f;
		float percentofblue =1.0f - ( y / segmentsize);
		unsigned char blue = static_cast<unsigned char>(percentofblue * 255.0f);
		Internal_ColorPicker.ColorArea->SetColors(MY_UI::Utilities::White, Utilities::Color(255, 0, 255, blue ), MY_UI::Utilities::Black, MY_UI::Utilities::Black);
	} else if(ypos < stripsize.y *5){
		y-=segmentsize*4.0f;	
		float percentofred = (y / segmentsize);
		unsigned char red= static_cast<unsigned char>(percentofred * 255.0f);
		Internal_ColorPicker.ColorArea->SetColors(MY_UI::Utilities::White, Utilities::Color(255, red, 255, 0 ), MY_UI::Utilities::Black, MY_UI::Utilities::Black);
	} else if(ypos < stripsize.y *6){
		y-=segmentsize*5.0f;
		float percentofgreen = 1.0f - (y / segmentsize);
		unsigned char green= static_cast<unsigned char>(percentofgreen * 255.0f);
		Internal_ColorPicker.ColorArea->SetColors(MY_UI::Utilities::White, Utilities::Color(255, 255, green, 0 ), MY_UI::Utilities::Black, MY_UI::Utilities::Black);
	} else {
		Internal_ColorPicker.ColorArea->SetColors(MY_UI::Utilities::White, Utilities::Color(255, 255, 0, 0 ), MY_UI::Utilities::Black, MY_UI::Utilities::Black);
	}
}
void MY_UI::Controls::ColorPickerWindow::DraggingCircle(){
	Utilities::Point p = Internal_ColorPicker.Circle->GetPos();
	p.x += New_MousePosx - Old_MousePosx;
	p.y += New_MousePosy - Old_MousePosy;
	Utilities::Point size = Internal_ColorPicker.ColorArea->GetSize();
	p.x = Utilities::Clamp<int>(p.x, -8, size.x -8);
	p.y = Utilities::Clamp<int>(p.y, -8, size.y -8);
	Internal_ColorPicker.Circle->SetPos(p.x, p.y);
	Update();
}
void MY_UI::Controls::ColorPickerWindow::MoveCircle(){
	Utilities::Point p(Utilities::TranslateToRelativeSpace(Internal_ColorPicker.Circle, New_MousePosx, New_MousePosy));
	Internal_ColorPicker.Circle->SetPos(p.x, p.y);
	Update();
}
void MY_UI::Controls::ColorPickerWindow::MoveToStrip(){
	Utilities::Point p(Utilities::TranslateToRelativeSpace(Internal_ColorPicker.Circle, New_MousePosx, New_MousePosy));
	Internal_ColorPicker.RightArrow->SetPos(Internal_ColorPicker.RightArrow->GetPos().x, p.y);
	DraggingArrow();
}
void MY_UI::Controls::ColorPickerWindow::SetPickedColor(Utilities::Color color){
	Internal_ColorPicker.PickedColor->SetColor(color);
	Internal_ColorPicker.Red = color.Red;
	Internal_ColorPicker.Blue = color.Blue;
	Internal_ColorPicker.Green = color.Green;
	ReverseColorApply();
}

void MY_UI::Controls::ColorPickerWindow::ReverseColorApply(){
	
	Utilities::Point stripsize = Internal_ColorPicker.ColorStrip_0->GetSize();
	float segmentsize = static_cast<float>(stripsize.y);
	Utilities::Point p = Internal_ColorPicker.RightArrow->GetPos();	
	
	float ypos= static_cast<float>(p.y);
	Utilities::Color color(255, Internal_ColorPicker.Red, Internal_ColorPicker.Green, Internal_ColorPicker.Blue); 
	int biggestcolor, middlecolor, smallestcolor;

	if(color.Red >= color.Blue){
		if(color.Green > color.Red){// size goes Green, Red, MY_UI::Utilities::Blue
			float percent =1.0f - static_cast<float>(color.Green)/255.0f;
			ypos = percent  * (segmentsize) + segmentsize*4.0f;		
			ypos += (static_cast<float>(color.Red)/255.0f) * (segmentsize);
			biggestcolor = color.Green;
			middlecolor = color.Red;
			smallestcolor = color.Blue;
		} else if(color.Blue > color.Green) { // size goes  Red, MY_UI::Utilities::Blue, Green
			float percent =1.0f - static_cast<float>(color.Red)/255.0f;
			ypos = percent  * (segmentsize);		
			ypos += static_cast<float>(color.Blue)/255.0f * (segmentsize);
			biggestcolor = color.Red;
			middlecolor = color.Blue;
			smallestcolor = color.Green;
		} else {// size goes  Red, Green, MY_UI::Utilities::Blue
			ypos =segmentsize*6.0f;
			ypos -= (static_cast<float>(color.Green)/255.0f) * (segmentsize);
			biggestcolor = color.Red;
			middlecolor = color.Green;
			smallestcolor = color.Blue;
		}
	}else if(color.Blue >= color.Green){
		if(color.Green > color.Red) { // MY_UI::Utilities::Blue, Green, Red
			float percent =1.0f - static_cast<float>(color.Blue)/255.0f;
			ypos = percent * (segmentsize) + segmentsize*2.0f;	
			ypos += (static_cast<float>(color.Green)/255.0f) * (segmentsize);
			biggestcolor = color.Blue;
			middlecolor = color.Green;
			smallestcolor = color.Red;
		} else { // MY_UI::Utilities::Blue, Red, Green

			float percent =1.0f - static_cast<float>(color.Blue)/255.0f;
			ypos = percent * (segmentsize) + segmentsize*2.0f;
			ypos -= (static_cast<float>(color.Red)/255.0f) * (segmentsize);

			biggestcolor = color.Blue;
			middlecolor =color.Red; 
			smallestcolor = color.Green;
		}
	} else { // Green, MY_UI::Utilities::Blue, Red	
		
		float percent =1.0f - static_cast<float>(color.Green)/255.0f;
		ypos = percent * (segmentsize) + segmentsize*4.0f;	
		biggestcolor = color.Green;
		middlecolor =color.Blue;
		smallestcolor = color.Red; 
	}

	float percenttobottom = 1.0f - static_cast<float>(biggestcolor)/255.0f;
	float percenttoleft = 1.0f - (static_cast<float>(smallestcolor)/(static_cast<float>(biggestcolor)+.1f));// in case of zero I added .1
	Utilities::Point circlesize = Internal_ColorPicker.Circle->GetSize();
	circlesize /=2;
	circlesize = -circlesize;
	Utilities::Point controlsize = Internal_ColorPicker.ColorArea->GetSize();
	Internal_ColorPicker.Circle->SetPos(static_cast<int>((percenttoleft * static_cast<float>(controlsize.x) ) ) +circlesize.x, static_cast<int>((percenttobottom * static_cast<float>(controlsize.y) )) +circlesize.y);

	Utilities::Point lp = Internal_ColorPicker.LeftArrow->GetPos();
	ypos -= Internal_ColorPicker.RightArrow->GetSize().y/2;
	Internal_ColorPicker.RightArrow->SetPos(p.x, static_cast<int>(ypos));
	Internal_ColorPicker.LeftArrow->SetPos(lp.x, static_cast<int>(ypos));
	Internal_ColorPicker.PickedColor->SetColor(color);// this is needed because the text boxes only modify the top left color, so I neeed to set all four points
	SetSliderFromArrows();
}





	