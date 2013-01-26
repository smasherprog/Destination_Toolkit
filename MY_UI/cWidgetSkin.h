#ifndef CWIDGETSKIN_H
#define CWIDGETSKIN_H
#include "assert.h"
#include "cWidgetRenderer.h"

#define SKIN_SIZE 1024.0f
#define TEXT_HEIGHT 30.0f
namespace MY_UI{


	class cWidgetSkin{
	public:

		Utilities::Texture UI_Skin;
		// Windows
		Utilities::Texture Window_With_Focus;	
		Utilities::Texture Window_Without_Focus;	
		Utilities::Texture WindowBar_With_Focus;
		Utilities::Texture WindowBar_Without_Focus;
		// window top right buttons unpressed
		Utilities::Texture Close_Window_Button;
		Utilities::Texture Minimize_Window_Button;
		Utilities::Texture Maximize_Window_Button;
		Utilities::Texture Restore_Window_Button;
		// window top right buttons pressed
		Utilities::Texture Close_Window_Button_Pressed;
		Utilities::Texture Minimize_Window_Button_Pressed;
		Utilities::Texture Maximize_Window_Button_Pressed;
		Utilities::Texture Restore_Window_Button_Pressed;

		Utilities::Texture CheckBox_Checked;
		Utilities::Texture CheckBox_Checked_Hovered;
		Utilities::Texture CheckBox_UnChecked;
		Utilities::Texture CheckBox_UnChecked_Hovered;

		Utilities::Texture Small_Button_Pressed;
		Utilities::Texture Small_Button_UnPressed;

		Utilities::Texture Medium_Button_Pressed;
		Utilities::Texture Medium_Button_UnPressed_Hovered;
		Utilities::Texture Medium_Button_UnPressed;

		Utilities::Texture Up_Arrow_Button;
		Utilities::Texture Up_Arrow_Button_Pressed;
		Utilities::Texture Down_Arrow_Button;
		Utilities::Texture Down_Arrow_Button_Pressed;
		Utilities::Texture Left_Arrow_Button;
		Utilities::Texture Left_Arrow_Button_Pressed;
		Utilities::Texture Right_Arrow_Button;
		Utilities::Texture Right_Arrow_Button_Pressed;

		Utilities::Texture Vertical_Scrollbar;
		Utilities::Texture Vertical_Scrollbar_Background;
		Utilities::Texture Horizontal_Scrollbar;
		Utilities::Texture Horizontal_Scrollbar_Background;

		Utilities::Texture TextBox;
		Utilities::Texture TextBox_Hovered;
		Utilities::Texture White_Box;
		Utilities::Texture White_Circle;
		Utilities::Texture Grey_Box;
		
		Utilities::Texture Selected_Box;

		Utilities::Texture Radio_Button;
		Utilities::Texture Radio_Button_Selected;
		Utilities::Texture Radio_Button_Hovered;
		Utilities::Texture Radio_Button_Selected_Hovered;

		Utilities::Texture Vertical_Slider;
		Utilities::Texture Horizontal_Slider;

		Utilities::Texture Right_Slider_Arrow;
		Utilities::Texture Up_Slider_Arrow;
		Utilities::Texture Down_Slider_Arrow;
		Utilities::Texture Left_Slider_Arrow;

		Utilities::Texture ListBox;

		Utilities::Texture Tab_With_Focus;
		Utilities::Texture Tab_WithOut_Focus;
		Utilities::Texture Tab_Hovered;

		Utilities::Texture Plus_Button_UnPressed;
		Utilities::Texture Plus_Button_Pressed;
		Utilities::Texture Minus_Button_UnPressed;
		Utilities::Texture Minus_Button_Pressed;
		Utilities::Texture Dot_Button_UnPressed;
		Utilities::Texture Dot_Button_Pressed;

		// a mapping of letters 
		std::vector<Utilities::Texture> Keyboard;

		cWidgetSkin()  { }	
		virtual ~cWidgetSkin() { DeInit(); }

		virtual bool Init(){

			UI_Skin.FileName ="..\\Assets\\MY_UI_Skin.png";
			bool b = Internal::WidgetRenderer->LoadTexture(&UI_Skin);
			assert(b);// must have the skin, otherwise... Crash baby.. crash!

			// THESE ARE ALL COPIES BELOW!!!.. DO NOT call Free only anything except for those called with Load
			SetUVsAndSize(UI_Skin, Window_With_Focus,		0.0f,	0.0f,	200.0f,	200.0f);	

			SetUVsAndSize(UI_Skin, WindowBar_With_Focus,	0.0f,	204.0f,	200.0f,	20.0f);
			SetUVsAndSize(UI_Skin, Horizontal_Scrollbar,	0.0f,	228,	200.0f,	16.0f);
			SetUVsAndSize(UI_Skin, Horizontal_Scrollbar_Background,	0.0f,	248,	200.0f,	16.0f);

			SetUVsAndSize(UI_Skin, Window_Without_Focus,	204.0f,	0.0f,	200.0f,	200.0f);
			SetUVsAndSize(UI_Skin, WindowBar_Without_Focus,	204.0f,	204.0f,	200.0f,	20.0f);

			SetUVsAndSize(UI_Skin, Medium_Button_Pressed,	283.0f,	228.0f,	75.0f,	23.0f);
			SetUVsAndSize(UI_Skin, Medium_Button_UnPressed,	204.0f,	228.0f,	75.0f,	23.0f);
			SetUVsAndSize(UI_Skin, Medium_Button_UnPressed_Hovered,	204.0f,	255.0f,	75.0f,	23.0f);

			SetUVsAndSize(UI_Skin, Vertical_Scrollbar,		448.0f,	0.0f,	16.0f,	200.0f);
			SetUVsAndSize(UI_Skin, Vertical_Scrollbar_Background,		468.0f,	0.0f,	16.0f,	200.0f);

			SetUVsAndSize(UI_Skin, Close_Window_Button,		408.0f, 0.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, Maximize_Window_Button,	408.0f,	18.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, Minimize_Window_Button,	408.0f,	36.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, Restore_Window_Button,	408.0f,	54.0f,	16.0f,	14.0f);			
			SetUVsAndSize(UI_Skin, CheckBox_UnChecked,		408.0f,	72.0f,	13.0f,	13.0f);

			SetUVsAndSize(UI_Skin, Small_Button_UnPressed,	408.0f, 89.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, Up_Arrow_Button,			408.0f,	107.0f,	16.0f,	16.0f);
			SetUVsAndSize(UI_Skin, Down_Arrow_Button,		408.0f,	127.0f,	16.0f,	16.0f);
			SetUVsAndSize(UI_Skin, Left_Arrow_Button,		408.0f,	147.0f,	16.0f,	16.0f);			
			SetUVsAndSize(UI_Skin, Right_Arrow_Button,		408.0f,	167.0f,	16.0f,	16.0f);

			SetUVsAndSize(UI_Skin, Close_Window_Button_Pressed,		428.0f, 0.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, Maximize_Window_Button_Pressed,	428.0f,	18.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, Minimize_Window_Button_Pressed,	428.0f,	36.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, Restore_Window_Button_Pressed,	428.0f,	54.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, CheckBox_Checked,				428.0f,	72.0f,	13.0f,	13.0f);

			SetUVsAndSize(UI_Skin, Small_Button_Pressed,			428.0f, 89.0f,	16.0f,	14.0f);
			SetUVsAndSize(UI_Skin, Up_Arrow_Button_Pressed,			428.0f,	107.0f,	16.0f,	16.0f);
			SetUVsAndSize(UI_Skin, Down_Arrow_Button_Pressed,		428.0f,	127.0f,	16.0f,	16.0f);
			SetUVsAndSize(UI_Skin, Left_Arrow_Button_Pressed,		428.0f,	147.0f,	16.0f,	16.0f);			
			SetUVsAndSize(UI_Skin, Right_Arrow_Button_Pressed,		428.0f,	167.0f,	16.0f,	16.0f);

			SetUVsAndSize(UI_Skin, TextBox,							362.0f,	228.0f,	102.0f,	23.0f);
			SetUVsAndSize(UI_Skin, TextBox_Hovered,					362.0f,	255.0f,	102.0f,	23.0f);
			
			SetUVsAndSize(UI_Skin, Selected_Box,					408.0f,	204.0f,	56.0f,	19.0f);

			SetUVsAndSize(UI_Skin, Vertical_Slider,					528.0f,	0.0f,	5.0f,	200.0f);
			SetUVsAndSize(UI_Skin, Horizontal_Slider,				0,	    268.0f, 200.0f,   5.0f);

			SetUVsAndSize(UI_Skin, Radio_Button,					488.0f,	0.0f,	12.0f,	12.0f);
			SetUVsAndSize(UI_Skin, Radio_Button_Selected,			508.0f,	0.0f,	12.0f,	12.0f);

			SetUVsAndSize(UI_Skin, Up_Slider_Arrow,					488.0f, 16.0f,   10.0f,  10.0f);
			SetUVsAndSize(UI_Skin, Right_Slider_Arrow,				508.0f,	16.0f,	 10.0f,	10.0f);

			SetUVsAndSize(UI_Skin, Down_Slider_Arrow,				488.0f, 29.0f,   10.0f,  10.0f);
			SetUVsAndSize(UI_Skin, Left_Slider_Arrow,				508.0f,	29.0f,	 10.0f,	10.0f);

			SetUVsAndSize(UI_Skin, CheckBox_UnChecked_Hovered,		488.0f, 42.0f,   13.0f,  13.0f);
			SetUVsAndSize(UI_Skin, CheckBox_Checked_Hovered,		508.0f,	42.0f,	 13.0f,	 13.0f);	

			SetUVsAndSize(UI_Skin, Radio_Button_Hovered,			488.0f, 59.0f,   12.0f,  12.0f);
			SetUVsAndSize(UI_Skin, Radio_Button_Selected_Hovered,	508.0f,	59.0f,	 12.0f,	 12.0f);	
			// white box made smaller on purpose to prevent linear texture sampling outside the box, which will make it blend with the background
			SetUVsAndSize(UI_Skin, White_Box,						490.0f,	77.0f,	12.0f,	12.0f);
			SetUVsAndSize(UI_Skin, White_Circle,					508.0f,	75.0f,	16.0f,	16.0f);
			SetUVsAndSize(UI_Skin, Grey_Box,						488.0f,	95.0f,	16.0f,	16.0f);

			SetUVsAndSize(UI_Skin, ListBox,							537.0f,	0.0f,	100.0f,	100.0f);
			
			SetUVsAndSize(UI_Skin, Tab_With_Focus,					0.0f,	277.0f,	81.0f,	22.0f);
			SetUVsAndSize(UI_Skin, Tab_Hovered,						85.0f,	277.0f,	81.0f,	22.0f);
			SetUVsAndSize(UI_Skin, Tab_WithOut_Focus,				0.0f,	303.0f,	81.0f,	22.0f);

			SetUVsAndSize(UI_Skin, Plus_Button_UnPressed,			488.0f,	115.0f,	12.0f,	12.0f);
			SetUVsAndSize(UI_Skin, Plus_Button_Pressed,				508.0f,	115.0f,	12.0f,	12.0f);
			SetUVsAndSize(UI_Skin, Minus_Button_UnPressed,			488.0f,	131.0f,	12.0f,	12.0f);
			SetUVsAndSize(UI_Skin, Minus_Button_Pressed,			508.0f,	131.0f,	12.0f,	12.0f);
			SetUVsAndSize(UI_Skin, Dot_Button_UnPressed,			488.0f,	147.0f,	12.0f,	12.0f);
			SetUVsAndSize(UI_Skin, Dot_Button_Pressed,				508.0f,	147.0f,	12.0f,	12.0f);


			//keyboard mapping... very long, but its a huge speed improvment to map all the keys 
			Keyboard.resize(256);// only support up to 256 characters
			Utilities::Texture temp, temp1;
			// starting with lower case a through z
			// i use a char to access the array so I can use a string directly to access the character mapping
			SetUVsAndSize(UI_Skin, temp,	0.0f,	1024 -30.0f,	13.0f,	30.0f);
			Keyboard['a'] = temp;
			temp1 = temp;
			float pxmaping[] = { 14.0f, 0.0f,	12.0f, 2.0f,	14.0f, 1.0f,	12.0f, 1.0f, // ends with e
				13.0f, 1.0f,	13.0f, 6.0f,	14.0f, 8.0f,	7.0f, 1.0f,		9.0f, 6.0f,  15.0f, 9.0f, //ends with k
				7.0f, 1.0f,		23.0f, 1.0f,	15.0f, 1.0f,	13.0f, 0.0f,	14.0f, 1.0f,	// ends with p
				14.0f, 2.0f,	10.0f, 1.0f,	10.0f, 1.0f,	9.0f, 1.0f,		15.0f, 0.0f, // ends with u
				15.0f, 0.0f,	19.0f, 0.0f,	14.0f, 1.0f,	15.0f, 8.0f,	12.0f, 1.0f,
				// this ends the lower case letters... beginning upper case
								18,	1,			19, 2,			20, 1,			18, 1,	// ends with E
				16, 0,			21, 2,			21, 0,			9, 1,			11, 1,		22, 0,	//ends with K
				18, 0,			26, 0,			23, 0,			20, 1,			17, 1, // ends with P
				20, 1,			20, 1,			14, 1,			17,2,			22, 0, // ends with U
				22, 1,			28, 0,			21, 0,			21, 1,			17, 1,
				//begin SHIFT KEYS
								3, 3,			25, 5,			15, 1,			13, 1, //ends with $
				23,2,			14,1,			22, 1,			12, 2,			9, 2, //ends with (
				9, 1,			16, 0,			16, 0,			9, 4,			9, 4, // ends with }
				3, 5,			4, 4,			12, 3,			17, 1,			17, 1,		14,1, // ends with ?
				// begin non shift keys
								8, 7,			14, 4,			12, 1,			14, 3,  // ends with 4
				12, 2,			14, 2,			14, 1,			12, 2,			14, 2, // ends with 9
				14, 1,			9, 2,			16, 3,			8, 2,			8, 1, // ends with ]
				10, 1,			5, 1,			5, 4,			5, 3,			4, 3,		10, 2
				

				};

			unsigned int i=0;
			for(char key('b'); key != 'z' +1; key++){
				SetUVsAndSizeForKeyboard(UI_Skin, temp,	temp1,	pxmaping[i], pxmaping[i +1]);
				Keyboard[key] = temp;
				temp1 = temp;
				i+=2;
			}
			
			SetUVsAndSize(UI_Skin, temp,	0.0f,	1024 -61.0f,	20.0f,	30.0f);
			Keyboard['A'] = temp;
			temp1 = temp;
			i=25*2;
			for(char key('B'); key != 'Z' +1; key++){
				SetUVsAndSizeForKeyboard(UI_Skin, temp,	temp1,	pxmaping[i], pxmaping[i +1]);
				Keyboard[key] = temp;
				temp1 = temp;
				i+=2;
			}

			SetUVsAndSizeForKeyboard(UI_Skin, temp,	Keyboard['z'], 16,	2);
			Keyboard['~'] = temp;
			temp1 = temp;
			char nextchars[] = {  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}',
				'|', ':', 34, '<', '>', '?' };

			i=50*2;
			for(unsigned int num(0); num < 20; num++){
				SetUVsAndSizeForKeyboard(UI_Skin, temp,	temp1,	pxmaping[i], pxmaping[i +1]);
				Keyboard[nextchars[num]] = temp;
				temp1 = temp;
				i+=2;
			}
			SetUVsAndSizeForKeyboard(UI_Skin, temp,	Keyboard['?'], 8,	1);
			Keyboard[' '] = temp;

			char lastchars [] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
				'[', ']', 92, ';',  39, ',', '.', '/' };

			SetUVsAndSizeForKeyboard(UI_Skin, temp,	Keyboard['Z'], 5,	7);
			Keyboard['`'] = temp;
			temp1 = temp;

			i=70*2;
			for(unsigned int num(0); num < 20; num++){
				SetUVsAndSizeForKeyboard(UI_Skin, temp,	temp1,	pxmaping[i], pxmaping[i +1]);
				Keyboard[lastchars[num]] = temp;
				temp1 = temp;
				i+=2;
			}

			return true;
		}
		virtual void DeInit(){
			Internal::WidgetRenderer->FreeTexture(&UI_Skin);// make sure to play nice and free the resources

		}
		// returns the size in pixels of the string passed
		MY_UI::Utilities::Point GetTextSize(std::string& text, float size){
	
			auto skin = MY_UI::Internal::WidgetSkin;
			int si = 0;
			float sizescale =size/TEXT_HEIGHT;
			Utilities::Point p(0, static_cast<int>(sizescale * skin->Keyboard['A'].height) );
			for(size_t i(0); i< text.size(); i++){	
				if(text[i] == '\n') {
					p.y += static_cast<int>((sizescale * skin->Keyboard['A'].height) + (size*1.0f/4.0f));// new line.. move down 
					p.x = p.x > si ? p.x : static_cast<int>(si);
					si = 0;
					continue;// continue to actual text
				}
				si += static_cast<int>(sizescale*Keyboard[text[i]].width);
			}			
			p.x = p.x > si ? p.x : si;
			return p;
		}
		MY_UI::Utilities::Point GetCharacterSize(char c, float size){
			float sizescale =size/TEXT_HEIGHT;
			return Utilities::Point(static_cast<int>(Keyboard[c].width * sizescale), static_cast<int>(size));
		}
		float GetCharacterWidth(char c, float size){ return Keyboard[c].width * size*(1.0f/TEXT_HEIGHT);  }
		
	protected:
		void SetUVsAndSize(Utilities::Texture& src, Utilities::Texture& uvs, float left, float top, float width, float height){
			uvs = src;// make a copy and overwrite the other stuff.
			uvs.width = width;
			uvs.height = height;
		
			uvs.v1 = top/SKIN_SIZE;	
			uvs.u1 = left/SKIN_SIZE;

			uvs.v2 = (top + height) / SKIN_SIZE;
			uvs.u2 = (left + width) / SKIN_SIZE;

		
		}
		void SetUVsAndSizeForKeyboard(Utilities::Texture& src, Utilities::Texture& uvs, Utilities::Texture& previous, float width, float pxspaces){
			uvs = src;// make a copy and overwrite the other stuff.
			uvs.width = width;
			uvs.height = previous.height;
		
			uvs.v1 = previous.v1;
			uvs.u1 = previous.u2 + (pxspaces/SKIN_SIZE);
				
			uvs.v2 = ((previous.v1*SKIN_SIZE) + uvs.height) / SKIN_SIZE;
			uvs.u2 = uvs.u1 + ( uvs.width / SKIN_SIZE );
		
		}
	};
};
#endif