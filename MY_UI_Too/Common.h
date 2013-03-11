#ifndef MY_UI_TOO_COMMON_H
#define MY_UI_TOO_COMMON_H


#define KEY_A int('A')
#define KEY_B int('B')
#define KEY_C int('C')
#define KEY_D int('D')
#define KEY_E int('E')
#define KEY_F int('F')
#define KEY_G int('G')
#define KEY_H int('H')
#define KEY_I int('I')
#define KEY_J int('J')
#define KEY_K int('K')
#define KEY_L int('L')
#define KEY_M int('M')
#define KEY_N int('N')
#define KEY_O int('O')
#define KEY_P int('P')
#define KEY_Q int('Q')
#define KEY_R int('R')
#define KEY_S int('S')
#define KEY_T int('T')
#define KEY_U int('U')
#define KEY_V int('V')
#define KEY_W int('W')
#define KEY_X int('X')
#define KEY_Y int('Y')
#define KEY_Z int('Z')

#define KEY_0 int('0')
#define KEY_1 int('1')
#define KEY_2 int('2')
#define KEY_3 int('3')
#define KEY_4 int('4')
#define KEY_5 int('5')
#define KEY_6 int('6')
#define KEY_7 int('7')
#define KEY_8 int('8')
#define KEY_9 int('9')


#define DOUBLE_CKICK_SPEED 200  // 200ms seconds is the time between clicks to be considered a double click
#define MAX_WINDOWSIZE 3000 // used to set the size of a full screen widget to ensure all mouse events are captured
#define POPUPTIME 600 // this is in miliseconds
#define START_CHAR '!'
#define END_CHAR '~'
#define NEXTLINEPERCENT .1f  //this is percentage of the tallest character used to padd between text lines
#define FONT_CREATE_SIZE 40 //this should provide font large enough for most uses.

const char Keyboard_Characters[] = {// this is padded strangly, but it is needed so I can use the VK_keys directly, the numbers and letter start at 48, and I want this struct to be const
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

enum Cursor_Types { Standard, Text, Mover, Tr_to_Bl,U_to_D, Tl_to_Br,  L_to_R, Hand};	
const LPSTR C_Types[] = { IDC_ARROW,  IDC_IBEAM,  IDC_SIZEALL, IDC_SIZENESW, IDC_SIZENS,  IDC_SIZENWSE, IDC_SIZEWE, IDC_HAND };

#define KEY_LEFT      VK_LEFT
#define KEY_RIGHT     VK_RIGHT
#define KEY_UP        VK_UP
#define KEY_DOWN      VK_DOWN
#define KEY_CTRL      VK_CONTROL
#define KEY_SHIFT     VK_SHIFT
#define KEY_RETURN    VK_RETURN
#define KEY_SPACE     VK_SPACE
#define KEY_TAB       VK_TAB
#define KEY_ESCAPE    VK_ESCAPE
#define KEY_BACK      VK_BACK
#define KEY_HOME      VK_HOME
#define KEY_END       VK_END
#define KEY_INSERT    VK_INSERT
#define KEY_DELETE    VK_DELETE

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

#define KEY_NUMPAD0 VK_NUMPAD0
#define KEY_NUMPAD1 VK_NUMPAD1
#define KEY_NUMPAD2 VK_NUMPAD2
#define KEY_NUMPAD3 VK_NUMPAD3
#define KEY_NUMPAD4 VK_NUMPAD4
#define KEY_NUMPAD5 VK_NUMPAD5
#define KEY_NUMPAD6 VK_NUMPAD6
#define KEY_NUMPAD7 VK_NUMPAD7
#define KEY_NUMPAD8 VK_NUMPAD8
#define KEY_NUMPAD9 VK_NUMPAD9

#define KEY_ADD        VK_ADD
#define KEY_SUBTRACT   VK_SUBTRACT
#define KEY_MULTIPLY   VK_MULTIPLY
#define KEY_DIVIDE     VK_DIVIDE
#define KEY_SEPARATOR  VK_SEPARATOR
#define KEY_DECIMAL    VK_DECIMAL
#define KEY_PAUSE      VK_PAUSE

#define MOUSE_LBUTTON MK_LBUTTON
#define MOUSE_RBUTTON MK_RBUTTON
#define MOUSE_MBUTTON MK_MBUTTON

#endif


#endif