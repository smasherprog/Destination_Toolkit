#ifndef MY_UI_TOO_COMMON_H
#define MY_UI_TOO_COMMON_H

#define DOUBLE_CKICK_SPEED 200  // 200ms seconds is the time between clicks to be considered a double click
#define MAX_WINDOWSIZE 3000 // used to set the size of a full screen widget to ensure all mouse events are captured
#define POPUPTIME 600 // this is in miliseconds
#define START_CHAR '!'
#define END_CHAR '~'
#define NEXTLINEPERCENT .1f  //this is percentage of the tallest character used to padd between text lines
#define FONT_CREATE_SIZE 30 //this should provide font large enough for most uses.

//used for creating the character map as it containes all characters
const char Keyboard_Characters[] = {// this is padded strangly, but it is needed so I can generate 
	'E','E','E','E','E','E','E','E','E','E',		'E','E','E','E','E','E','E','E','E','E',		
	'E','E','E','E','E','E','E','E','E','E',		'E','E',' ','!','\"','#','$','%','&','\'',		'(',')','*','+','`','-','.','/',
	// above is padding, the number 0 starts at  0x30 
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // the number 9 represents the decimal number 57, or 0x39, 
	':',';','<','=','>','?','@',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L','M','N','O','P', 'Q', 'R', 'S', 'T' , 'U', 'V', 'W', 'X', 'Y', 'Z',
	'[','\\',']','^','_','`',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l','m','n','o','p', 'q', 'r', 's', 't' , 'u', 'v', 'w', 'x', 'y', 'z',
	'{','|','}','~'
};
	
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#define KEY_LEFT      VK_LEFT
#define KEY_UP        VK_UP
#define KEY_RIGHT     VK_RIGHT
#define KEY_DOWN      VK_DOWN

#define KEY_ESCAPE    VK_ESCAPE

#define KEY_F1  VK_F1
#define KEY_F2  VK_F2
#define KEY_F3  VK_F3
#define KEY_F4  VK_F4
#define KEY_F5  VK_F5
#define KEY_F6  VK_F6
#define KEY_F7  VK_F7
#define KEY_F8  VK_F8
#define KEY_F9  VK_F9
#define KEY_F10 VK_F10
#define KEY_F11 VK_F11
#define KEY_F12 VK_F12

#define KEY_DELETE VK_DELETE

enum Cursor_Types { Standard, Text, Mover, Tr_to_Bl,U_to_D, Tl_to_Br,  L_to_R, Hand};	
const LPSTR C_Types[] = { IDC_ARROW,  IDC_IBEAM,  IDC_SIZEALL, IDC_SIZENESW, IDC_SIZENS,  IDC_SIZENWSE, IDC_SIZEWE, IDC_HAND };

#endif


#endif