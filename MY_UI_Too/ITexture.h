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
		};
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
			virtual Utilities::Point Get_Dimensions(){ return _Dimensions;}

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