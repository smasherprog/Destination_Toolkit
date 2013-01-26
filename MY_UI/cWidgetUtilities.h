#ifndef CWIDGETUTILITIES_H
#define CWIDGETUTILITIES_H
#include <string>
#include <iostream>

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#pragma comment( lib, "winmm" )

#endif

#ifndef OUTPUT_DEBUG_MSG // nothing special about this, I use the console for standard debug info
#define OUTPUT_DEBUG_MSG(x) {												\
	std::cout<<x<<std::endl;													     	\
}
#endif

// no need to check if(x), in c++, delete always checks that before deleting
#ifndef RELEASECOM
#define RELEASECOM(x) { if(x) x->Release(); x=0; }
#endif
#ifndef DELETE_ARRAY
#define DELETE_ARRAY(x) { delete[] (x);  x=NULL; } 
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { delete (x);  x=NULL; } 
#endif

#ifdef  NDEBUG
#define assert2(_Expression, _Msg) ((void)0)
#else 
#define assert2(_Expression, _Msg) (void)( (!!(_Expression)) || (_wassert(_Msg, _CRT_WIDE(__FILE__), __LINE__), 0) )
#endif

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

extern bool Mouse_RButton_Down, Mouse_LButton_Down, Mouse_MButton_Down;
extern bool CapsLock, ShiftDown, CtrlDown, NumLock, Key_Down;

// old_mouse pos is the position of the mouse cursor on the last loop. So, you can subtract the Old-New to get the delta to move things on screen
extern int Old_MousePosx, Old_MousePosy, New_MousePosx, New_MousePosy, Delta_Mousex, Delta_Mousey;
extern int Mouse_Wheel_Delta; // mouse wheel is always a delta
extern unsigned int Current_Cursor;
extern int Current_Key;
#define DOUBLE_CKICK_SPEED 200  // 200ms seconds is the time between clicks to be considered a double click
#define MAX_WINDOWSIZE 3000 // used to set the size of a full screen widget to ensure all mouse events are captured

const char LowerChars[256] = {// this is padded strangly, but it is needed so I can use the VK_keys directly, the numbers and letter start at 48, and I want this struct to be const
	'E','E','E','E','E','E','E','E','E','E',		'E','E','E','E','E','E','E','E','E','E',		
	'E','E','E','E','E','E','E','E','E','E',		'E','E',' ','E','E','E','E','E','E','E',		'E','E','E','E','E','E','E','E',
	// above is padding, the number 0 starts at  0x30 
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // the number 9 represents the decimal number 57, or 0x39, 

	'E','E','E','E','E','E','E',// these are padding because the next characters start at decimal 65 

	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l','m','n','o','p', 'q', 'r', 's', 't' , 'u', 'v', 'w', 'x', 'y', 'z'
};
// upper chars are used for Capslock
const char UpperChars[256] = { 
	'E','E','E','E','E','E','E','E','E','E',		'E','E','E','E','E','E','E','E','E','E',		
	'E','E','E','E','E','E','E','E','E','E',		'E','E',' ','E','E','E','E','E','E','E',		'E','E','E','E','E','E','E','E',
	// above is padding, the number 0 starts at  0x30 
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // the number 9 represents the decimal number 57, or 0x39, 

	'E','E','E','E','E','E','E',// these are padding because the next characters start at deciman 65 

	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L','M','N','O','P', 'Q', 'R', 'S', 'T' , 'U', 'V', 'W', 'X', 'Y', 'Z'
};

// shiftchars are used for holding shift down
const char ShiftChars[256] = { 
	'E', 'E', '!', '@', '#', '$', '%', '^', '&', '*', 
	'(', ')', '_', '+', 'E', 'E', 'Q', 'W', 'E', 'R', 
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 'E', 'E',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
	34, 'E', 'E', 124, 'Z', 'X', 'C', 'V', 'B', 'N', 
	'M', '<', '>', '?', 'E', 'E','E', ' '
};	

namespace MY_UI{
	namespace Controls{
		class cWidgetBase;
	};
	namespace Utilities{
		// the alignments can be OR'd together
#define NO_ALIGNMENT 0
#define LEFT 1
#define RIGHT 2
#define TOP 4
#define BOTTOM 8
#define CENTER 16
#define IN_LINE 32
#define FILL 64	

		class Color{
		public:
			union{
				struct{
					unsigned char  Red, Green, Blue, Alpha;
				};
				unsigned int color;
			};

			Color() : Alpha(255), Red(255), Green(255), Blue(255) {}
			Color(unsigned char a, unsigned char r, unsigned char g, unsigned char b) : Alpha(a), Red(r), Green(g), Blue(b) {}
			inline void operator=(const Color& r){ Alpha=r.Alpha; Red=r.Red; Green = r.Green; Blue = r.Blue; }
			inline void operator+=(const Color& r){ Alpha+=r.Alpha; Red+=r.Red; Green += r.Green; Blue += r.Blue; }
			inline void operator+=(const unsigned char& r){ Alpha+=r; Red+=r; Green += r; Blue += r; }
			inline void operator-=(const Color& r){ Alpha-=r.Alpha; Red-=r.Red; Green -= r.Green; Blue -= r.Blue; }
			inline void operator-=(const unsigned char& r){ Alpha-=r; Red-=r; Green -= r; Blue -= r; }
			inline void operator*=(const Color& r){Alpha*=r.Alpha; Red*=r.Red; Green *= r.Green; Blue *= r.Blue; }
			inline void operator*=(const unsigned char& r){ Alpha*=r; Red*=r; Green *= r; Blue *= r; }
			inline void operator*=(const float& r){ Alpha=static_cast<unsigned char>(static_cast<float>(Alpha)*r); Red = static_cast<unsigned char>(static_cast<float>(Red)*r); Green = static_cast<unsigned char>(static_cast<float>(Green) * r); Blue = static_cast<unsigned char>(static_cast<float>(Blue )*r ); }
			inline void operator/=(const Color& r){Alpha/=r.Alpha; Red/=r.Red; Green /= r.Green; Blue /= r.Blue; }
			inline void operator/=(const unsigned char& r){ Alpha/=r; Red/=r; Green /= r; Blue /= r; }
			inline void operator|=(const Color& r){ Alpha|=r.Alpha; Red|=r.Red; Green |= r.Green; Blue |= r.Blue; }
			inline void operator&=(const Color& r){ Alpha&=r.Alpha; Red&=r.Red; Green &= r.Green; Blue &= r.Blue; }

		};
		inline Color operator+(const Color& l, const Color& r){ return Color(l.Alpha+r.Alpha, l.Red+r.Red, l.Green+r.Green, l.Blue+r.Blue); }
		inline Color operator+(const Color& l, const unsigned char& r){ return Color(l.Alpha+r, l.Red+r, l.Green+r, l.Blue+r); }
		inline Color operator-(const Color& l, const Color& r){ return Color(l.Alpha-r.Alpha, l.Red-r.Red, l.Green-r.Green, l.Blue-r.Blue); }
		inline Color operator-(const Color& l, const unsigned char& r){ return Color(l.Alpha-r, l.Red-r, l.Green-r, l.Blue-r); }
		inline Color operator*(const Color& l, const Color& r){ return Color(l.Alpha*r.Alpha, l.Red*r.Red, l.Green*r.Green, l.Blue*r.Blue); }
		inline Color operator*(const Color& l, const unsigned char& r){ return Color(l.Alpha*r, l.Red*r, l.Green*r, l.Blue*r); }
		inline Color operator*(const Color& l, const float& r){ return Color(static_cast<unsigned char>(static_cast<float>(l.Alpha)*r), static_cast<unsigned char>(static_cast<float>(l.Red)*r), static_cast<unsigned char>(static_cast<float>(l.Green)*r), static_cast<unsigned char>(static_cast<float>(l.Blue)*r)); }
		inline Color operator/(const Color& l, const Color& r){ return Color(l.Alpha/r.Alpha, l.Red/r.Red, l.Green/r.Green, l.Blue/r.Blue); }
		inline Color operator/(const Color& l, const unsigned char& r){ return Color(l.Alpha/r, l.Red/r, l.Green/r, l.Blue/r); }
		inline Color operator|(const Color& l, const Color& r){ return Color(l.Alpha|r.Alpha, l.Red|r.Red, l.Green|r.Green, l.Blue|r.Blue); }
		inline Color operator&(const Color& l, const Color& r){ return Color(l.Alpha&r.Alpha, l.Red&r.Red, l.Green&r.Green, l.Blue&r.Blue); }

		const MY_UI::Utilities::Color Red(255, 255, 0, 0);
		const MY_UI::Utilities::Color Green(255, 0, 255, 0);
		const MY_UI::Utilities::Color Blue(255, 0, 0, 255);
		const MY_UI::Utilities::Color Black(255, 0, 0, 0);
		const MY_UI::Utilities::Color White(255, 255, 255, 255);
		class Point{
		public:
			// the uinion is just for naming purposes
			union{
				struct{
					int left, top;
				};
				struct{
					int x, y;
				};
			};
			Point() : left(0), top(0) {}
			Point(int l, int t) : left(l), top(t) {}

			inline void operator=(const Point& r){ left=r.left; top=r.top; }
			inline void operator+=(const Point& r){ left+=r.left; top+=r.top; }
			inline void operator+=(const int& r){ left+=r; top+=r; }
			inline void operator-=(const Point& r){ left-=r.left; top-=r.top; }
			inline void operator-=(const int& r){ left-=r; top-=r; }
			inline void operator*=(const Point& r){ left*=r.left; top*=r.top; }
			inline void operator*=(const int& r){ left*=r; top*=r; }
			inline void operator*=(const float& r){ left=static_cast<int>(static_cast<float>(left)*r); top=static_cast<int>(static_cast<float>(top)*r); }
			inline void operator/=(const Point& r){ left/=r.left; top/=r.top; }
			inline void operator/=(const int& r){ left/=r; top/=r; }

		};
		inline Point operator+(const Point& l, const Point& r){ return Point(l.left+r.left, l.top+r.top); }
		inline Point operator-(const Point& l, const Point& r){ return Point(l.left-r.left, l.top-r.top); }
		inline Point operator*(const Point& l, const Point& r){ return Point(l.left*r.left, l.top*r.top); }
		inline Point operator/(const Point& l, const Point& r){ return Point(l.left/r.left, l.top/r.top); }
		inline Point operator-(const Point& l){ return Point(-l.left, -l.top); }
		class Rect{
		public:
			union{
				struct{
					int left, top, width, height;
				};
				int Points[4];
			};
			Rect() : left(0), top(0), width(0), height(0) {}
			Rect(unsigned int l, unsigned int t, unsigned int w, unsigned int h) : left(l), top(t), width(w), height(h) {}
			Rect(Point tl, Point wh) : left(tl.left), top(tl.top), width(wh.left), height(wh.y) {}
			inline void operator=(const Rect& r){ left=r.left; top=r.top; width=r.width; height=r.height; }
			inline void operator+=(const Rect& r){ left+=r.left; top+=r.top; width+=r.width; height+=r.height; }
			inline void operator+=(const int& r){ left+=r; top+=r; width+=r; height+=r; }
			inline void operator-=(const Rect& r){ left-=r.left; top-=r.top; width-=r.width; height-=r.height; }
			inline void operator-=(const int& r){ left-=r; top-=r; width-=r; height-=r; }
			inline void operator*=(const Rect& r){ left*=r.left; top*=r.top; width*=r.width; height*=r.height; }
			inline void operator*=(const int& r){ left*=r; top*=r; width*=r; height*=r; }
			inline void operator/=(const Rect& r){ left/=r.left; top/=r.top; width/=r.width; height/=r.height; }
			inline void operator/=(const int& r){ left/=r; top/=r; width/=r; height/=r; }


			inline bool Within(const Point& p) const { return (p.left >= left) & (p.left <= left + width) & (p.top >= top) & (p.top <= top + height);  }
		};
		inline Rect operator+(const Rect& l, const Rect& r){ return Rect(l.left+r.left, l.top+r.top, l.width+r.width, l.height+r.height); }
		inline Rect operator-(const Rect& l, const Rect& r){ return Rect(l.left-r.left, l.top-r.top, l.width-r.width, l.height-r.height); }
		inline Rect operator*(const Rect& l, const Rect& r){ return Rect(l.left*r.left, l.top*r.top, l.width*r.width, l.height*r.height); }
		inline Rect operator/(const Rect& l, const Rect& r){ return Rect(l.left/r.left, l.top/r.top, l.width/r.width, l.height/r.height); }
		inline Rect operator-(const Rect& l){ return Rect(-l.left, -l.top, -l.width, -l.height); }
		inline std::ostream& operator<<(std::ostream& strem, const Rect& obj){
			strem<<"left="<<obj.left<<"  top="<<obj.top<<"   width="<<obj.width<<"  height="<<obj.height<<std::endl;
			return strem;
		}

		inline unsigned int Color_ARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b) {
			return ((unsigned int)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)));
		}
		inline std::ostream& operator<<(std::ostream& strem, const Point& obj){
			strem<<"left="<<obj.x<<"  top="<<obj.y<<std::endl;
			return strem;
		}
		struct UVs{
			float u1, v1; //u1 = left, v1 = top
			float u2, v2;//u2 = right, v2 = bottom
		};
		class Texture{
		public:
			// uvs so peices of the texture can be accessed instead of the entire texture
			struct {
				float u1, v1; //u1 = left, v1 = top
				float u2, v2;//u2 = right, v2 = bottom
			};

			std::string	FileName;
			void*	Tex;// use this pointer to store the texture pointer, just cast it into what ever type is needed
			float	width,	height;
			unsigned int id;// this is not used, but in some systems like OpenGl, objects are issued an unsigned int to reference the object. So, users can store that here
			Texture(): Tex(0), width(4.0f), height(4.0f), id(-1), u1(0.0f), v1(0.0f), u2(1.0f), v2(1.0f) { }
			void clear();

		};
		inline void CopyUvs(MY_UI::Utilities::UVs& uvs, const MY_UI::Utilities::Texture& tex){
			uvs.u1 = tex.u1;
			uvs.v1 = tex.v1;
			uvs.u2 = tex.u2;
			uvs.v2 = tex.v2;
		}
		inline void CopyUvs(MY_UI::Utilities::Texture tex, const MY_UI::Utilities::UVs& uvs){
			tex.u1=uvs.u1;
			tex.v1=uvs.v1;
			tex.u2=uvs.u2;
			tex.v2=uvs.v2;
		}
		class Font{	
		public:
			std::string	FontName;
			float size;
			bool  bold, dropshadow;
			void*	FontData;// this should store a structure to all the necessary info to draw text. It can differ from platform to platform, which is why the pointer is void. Cast awawy!!
			Font(): FontData(0), FontName("Arial"), size(15), dropshadow(false), bold(false) {}
			void clear();

		};
		template<typename T>T Clamp(T Value, T Min, T Max){
			return (Value < Min) ? Min : (Value > Max) ? Max : Value;
		}

		inline Utilities::Color Lerp(const Utilities::Color &beg, const Utilities::Color &end, float &scalar){ return beg + (end - beg) * scalar; }
		MY_UI::Utilities::Point TranslateToRelativeSpace(MY_UI::Controls::cWidgetBase* widget, int x, int y);

		unsigned int GetTime();
		enum Cursor_Types { Standard, Text, Mover, Tr_to_Bl,U_to_D, Tl_to_Br,  L_to_R};	
	};


#if defined (_WINDOWS) | defined (_WIN64) | defined (WIN32)

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
	namespace Utilities{
		const LPSTR C_Types[] = { IDC_ARROW,  IDC_IBEAM,  IDC_SIZEALL, IDC_SIZENESW, IDC_SIZENS,  IDC_SIZENWSE, IDC_SIZEWE };
		inline void SetCursor(unsigned int type){ ::SetCursor( LoadCursor(0, C_Types[type]) ); Current_Cursor = type;}
		inline unsigned int GetCursor(){ return Current_Cursor; }
		class cInput{
			// make sure there are no copies made!
			cInput(const cInput& rhs);
			cInput& operator=(const cInput& rhs);

		public:

			bool ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			bool AppPaused, Minimized, Maximized, Resizing;
			cInput(): AppPaused(false), Minimized(false), Maximized(false), Resizing(false) {}// this should be the ONLY way this is constructed 
			~cInput();

		};
	};

#endif
};

#endif