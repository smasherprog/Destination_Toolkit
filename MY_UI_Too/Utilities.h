#ifndef MY_UI_TOO_UTILITIIES_H
#define MY_UI_TOO_UTILITIIES_H
#include <string>
#include <iostream>
#include "../Utilities/Utilities.h"

namespace MY_UI_Too{
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

		const MY_UI_Too::Utilities::Color Red(255, 255, 0, 0);
		const MY_UI_Too::Utilities::Color Green(255, 0, 255, 0);
		const MY_UI_Too::Utilities::Color Blue(255, 0, 0, 255);
		const MY_UI_Too::Utilities::Color Black(255, 0, 0, 0);
		const MY_UI_Too::Utilities::Color White(255, 255, 255, 255);
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
			Point(int left, int top) : left(left), top(top) {}

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

			struct{
				int left, top, width, height;
			};

			Rect() : left(0), top(0), width(0), height(0) {}
			Rect(int left, int top, int width, int height) : left(left), top(top), width(width), height(height) {}
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

			inline bool Intersect(const Point& p) const { return (p.left >= left) & (p.left <= left + width) & (p.top >= top) & (p.top <= top + height);  }
			inline bool Intersect(const Rect& p) const { 
				if(left > p.left + p.width) return false;
				else if(left + width < p.left) return false;
				else if(top > p.top + p.height) return false;
				else if(top + height < p.top) return false;
				return true;
			}
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

		template<typename T>T Clamp(T Value, T Min, T Max){
			return (Value < Min) ? Min : (Value > Max) ? Max : Value;
		}

		inline Utilities::Color Lerp(const Utilities::Color &beg, const Utilities::Color &end, float &scalar){ return beg + (end - beg) * scalar; }

		unsigned int GetTime();
		enum Cursor_Types { Standard, Text, Mover, Tr_to_Bl,U_to_D, Tl_to_Br,  L_to_R};	
	};


};


#endif