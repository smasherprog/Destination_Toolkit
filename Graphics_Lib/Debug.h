
#if defined(_DEBUG) | defined (DEBUG)
	#include <dxerr.h>
	#include <iostream>
	#include <assert.h>
	#ifndef HR
	#define HR(x){																\
		HRESULT hr = (x);														\
		if(FAILED(hr)){															\
		std::cout<<DXGetErrorStringA(hr)<<std::endl<<DXGetErrorDescriptionA(hr)<<std::endl;   \
		assert(0);															\
		}																		\
			}
	#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif