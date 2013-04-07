#ifndef MY_UI_TOO_ITEXTURE_H
#define MY_UI_TOO_ITEXTURE_H
#include <string>
#include "Utilities.h"

namespace MY_UI_Too{

	namespace Utilities{
		class UVs{
		public:
			float u1, v1; //u1 = left, v1 = top
			float u2, v2;//u2 = right, v2 = bottom
			UVs():u1(0.0f), v1(0.0f), u2(1.0f), v2(1.0f){}
			UVs(const MY_UI_Too::Utilities::Rect r):u1((float)r.left), v1((float)r.top), u2((float)(r.left + r.width)), v2((float)(r.top + r.height)){}
			UVs(float _u1, float _v1,float _u2,float _v2):u1(_u1), v1(_v1), u2(_u2), v2(_v2){}
			void operator *=(float scaler) {u1*=scaler; v1*=scaler;u2*=scaler;v2*=scaler;}
			void operator +=(UVs uvs) {u1+=uvs.u1; v1+=uvs.v1;u2+=uvs.u2;v2+=uvs.v2;}
			UVs& operator =(const MY_UI_Too::Utilities::Rect r) {u1=(float)r.left; v1=(float)r.top;u2=(float)(r.left + r.width);v2=(float)(r.top + r.height); return *this;}
		};
		inline bool operator==(const UVs& l, const UVs& r){return (l.u1==r.u1) & (l.u2==r.u2) & (l.v1==r.v1) & (l.v2==r.v2); };
		inline UVs operator*(const UVs& l, const float scaler){ return UVs(l.u1*scaler, l.v1*scaler,l.u2*scaler,l.v2*scaler); };
	};
	namespace Interfaces{
		class ITexture{
		protected:
			std::string	_FileName;
			Utilities::Point _Dimensions;
			void* _Texture;
			void* _Render_Texture;
		public:

			ITexture():_Texture(nullptr), _Render_Texture(nullptr) {}
			virtual ~ITexture(){}

			virtual std::string Get_FileName() const { return _FileName; }
			virtual void Set_FileName(std::string filename) {  _FileName=filename; }

			virtual void Set_Dimensions(Utilities::Point dimensions){ _Dimensions = dimensions;}
			virtual Utilities::Point Get_Dimensions() const{ return _Dimensions;}

			// this had to be a void* to stay independant of implenetation. The derived classes will have to cast to the proper type for using
			virtual void* Get_Texture() const { return _Texture; }
			virtual void Set_Texture(void* texture)  {  _Texture =texture; }

			virtual void* Get_Render_Texture() const { return _Render_Texture; }
			virtual void Set_Render_Texture(void* texture)  {  _Render_Texture =texture; }

			virtual void clear()=0;

		};
	};
};
#endif