#include "stdafx.h"
#include "Globals_GraphicsCore.h"


const DXGI_FORMAT formats[] = {
	DXGI_FORMAT_UNKNOWN,

	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_UNKNOWN, // RGB8 not directly supported
	DXGI_FORMAT_R8G8B8A8_UNORM,

	DXGI_FORMAT_R16_UNORM, 
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_UNKNOWN, // RGB16 not directly supported
	DXGI_FORMAT_R16G16B16A16_UNORM,

	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_UNKNOWN, // RGB8S not directly supported
	DXGI_FORMAT_R8G8B8A8_SNORM,

	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_UNKNOWN, // RGB16S not directly supported
	DXGI_FORMAT_R16G16B16A16_SNORM,

	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_UNKNOWN, // RGB16F not directly supported
	DXGI_FORMAT_R16G16B16A16_FLOAT,

	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,

	DXGI_FORMAT_R16_SINT,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_UNKNOWN, // RGB16I not directly supported
	DXGI_FORMAT_R16G16B16A16_SINT,

	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R32G32_SINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R32G32B32A32_SINT,

	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_UNKNOWN, // RGB16UI not directly supported
	DXGI_FORMAT_R16G16B16A16_UINT,

	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32A32_UINT,
//50
	DXGI_FORMAT_UNKNOWN, // RGBE8 not directly supported
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
	DXGI_FORMAT_R11G11B10_FLOAT,
	DXGI_FORMAT_B5G6R5_UNORM,
	DXGI_FORMAT_UNKNOWN, // RGBA4 not directly supported
	DXGI_FORMAT_R10G10B10A2_UNORM,

	DXGI_FORMAT_D16_UNORM,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D32_FLOAT,

	DXGI_FORMAT_BC1_UNORM,
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC3_UNORM,
	DXGI_FORMAT_BC4_UNORM,
	DXGI_FORMAT_BC5_UNORM,

	DXGI_FORMAT_R32G32_TYPELESS
};
const float DXGI_FormatStride[] ={
   0,
    16,//DXGI_FORMAT_R32G32B32A32_TYPELESS       = 16,
    16,//DXGI_FORMAT_R32G32B32A32_FLOAT          = 16,
    16,//DXGI_FORMAT_R32G32B32A32_UINT           = 16,
    16,//DXGI_FORMAT_R32G32B32A32_SINT           = 16,
    12,//DXGI_FORMAT_R32G32B32_TYPELESS          = 12,
    12,//DXGI_FORMAT_R32G32B32_FLOAT             = 12,
    12,//DXGI_FORMAT_R32G32B32_UINT              = 12,
    12,//DXGI_FORMAT_R32G32B32_SINT              = 12,
    8,//DXGI_FORMAT_R16G16B16A16_TYPELESS       = 8,
    8,//DXGI_FORMAT_R16G16B16A16_FLOAT          = 8,
    8,//DXGI_FORMAT_R16G16B16A16_UNORM          = 8,
    8,//DXGI_FORMAT_R16G16B16A16_UINT           = 8,
    8,//DXGI_FORMAT_R16G16B16A16_SNORM          = 8,
    8,//DXGI_FORMAT_R16G16B16A16_SINT           = 8,
    8,//DXGI_FORMAT_R32G32_TYPELESS             = 8,
    8,//DXGI_FORMAT_R32G32_FLOAT                = 8,
    8,//DXGI_FORMAT_R32G32_UINT                 = 8,
    8,//DXGI_FORMAT_R32G32_SINT                 = 8,
    8,//DXGI_FORMAT_R32G8X24_TYPELESS           = 8,
    8,//DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 8,
    8,//DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 8,
    8,//DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 8,
    4,//DXGI_FORMAT_R10G10B10A2_TYPELESS        = 4,
    4,//DXGI_FORMAT_R10G10B10A2_UNORM           = 4,
    4,//DXGI_FORMAT_R10G10B10A2_UINT            = 4,
    4,//DXGI_FORMAT_R11G11B10_FLOAT             = 4,
    4,//DXGI_FORMAT_R8G8B8A8_TYPELESS           = 4,
    4,//DXGI_FORMAT_R8G8B8A8_UNORM              = 4,
    4,//DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 4,
    4,//DXGI_FORMAT_R8G8B8A8_UINT               = 4,
    4,//DXGI_FORMAT_R8G8B8A8_SNORM              = 4,
    4,//DXGI_FORMAT_R8G8B8A8_SINT               = 4,
    4,//DXGI_FORMAT_R16G16_TYPELESS             = 4,
    4,//DXGI_FORMAT_R16G16_FLOAT                = 4,
    4,//DXGI_FORMAT_R16G16_UNORM                = 4,
    4,//DXGI_FORMAT_R16G16_UINT                 = 4,
    4,//DXGI_FORMAT_R16G16_SNORM                = 4,
    4,//DXGI_FORMAT_R16G16_SINT                 = 4,
    4,//DXGI_FORMAT_R32_TYPELESS                = 4,
    4,//DXGI_FORMAT_D32_FLOAT                   = 4,
    4,//DXGI_FORMAT_R32_FLOAT                   = 4,
    4,//DXGI_FORMAT_R32_UINT                    = 4,
    4,//DXGI_FORMAT_R32_SINT                    = 4,
    4,//DXGI_FORMAT_R24G8_TYPELESS              = 4,
    4,//DXGI_FORMAT_D24_UNORM_S8_UINT           = 4,
    4,//DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 4,
    4,//DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 
    2,//DXGI_FORMAT_R8G8_TYPELESS               = 2,
    2,//DXGI_FORMAT_R8G8_UNORM                  = 2,
    2,//DXGI_FORMAT_R8G8_UINT                   = 2,
    2,//DXGI_FORMAT_R8G8_SNORM                  = 2,
    2,//DXGI_FORMAT_R8G8_SINT                   = 2,
    2,//DXGI_FORMAT_R16_TYPELESS                = 2,
    2,//DXGI_FORMAT_R16_FLOAT                   = 2,
    2,//DXGI_FORMAT_D16_UNORM                   = 2,
    2,//DXGI_FORMAT_R16_UNORM                   = 
    2,//DXGI_FORMAT_R16_UINT                    = 
    2,//DXGI_FORMAT_R16_SNORM                   = 
    2,//DXGI_FORMAT_R16_SINT                    = 
    1,//DXGI_FORMAT_R8_TYPELESS                 = 
    1,//DXGI_FORMAT_R8_UNORM                    =
    1,//DXGI_FORMAT_R8_UINT                     = 
    1,//DXGI_FORMAT_R8_SNORM                    = 
    1,//DXGI_FORMAT_R8_SINT                     = 
    1,//DXGI_FORMAT_A8_UNORM                    = 
    1,//DXGI_FORMAT_R1_UNORM                    = 
    4,//DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 
    4,//DXGI_FORMAT_R8G8_B8G8_UNORM             = 
    4,//DXGI_FORMAT_G8R8_G8B8_UNORM             = 
    .5f,//DXGI_FORMAT_BC1_TYPELESS                = 
    .5f,//DXGI_FORMAT_BC1_UNORM                   = 
    .5f,//DXGI_FORMAT_BC1_UNORM_SRGB              = 
    1,//DXGI_FORMAT_BC2_TYPELESS                = 
    1,//DXGI_FORMAT_BC2_UNORM                   = 
    1,//DXGI_FORMAT_BC2_UNORM_SRGB              = 
    1,//DXGI_FORMAT_BC3_TYPELESS                = 
    1,//DXGI_FORMAT_BC3_UNORM                   = 
    1,//DXGI_FORMAT_BC3_UNORM_SRGB              = 
    1,//DXGI_FORMAT_BC4_TYPELESS                = 
    1,//DXGI_FORMAT_BC4_UNORM                   = 
    1,//DXGI_FORMAT_BC4_SNORM                   = 
    1,//DXGI_FORMAT_BC5_TYPELESS                = 
    1,//DXGI_FORMAT_BC5_UNORM                   = 
    1,//DXGI_FORMAT_BC5_SNORM                   = 
    2,//DXGI_FORMAT_B5G6R5_UNORM                = 
    2,//DXGI_FORMAT_B5G5R5A1_UNORM              = 
    4,//DXGI_FORMAT_B8G8R8A8_UNORM              = 
    4,//DXGI_FORMAT_B8G8R8X8_UNORM              = 
    4,//DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 
    4,//DXGI_FORMAT_B8G8R8A8_TYPELESS           = 
    4,//DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 
    4,//DXGI_FORMAT_B8G8R8X8_TYPELESS           = 
    4,//DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 
    4,//DXGI_FORMAT_BC6H_TYPELESS               = 
    4,//DXGI_FORMAT_BC6H_UF16                   = 
    4,//DXGI_FORMAT_BC6H_SF16                   =
    4,//DXGI_FORMAT_BC7_TYPELESS                =
    4,//DXGI_FORMAT_BC7_UNORM                   = 
    4, //DXGI_FORMAT_BC7_UNORM_SRGB              =
};
const float FormatStride[FORMAT_RG32TL +1] = {
	0,

	// Unsigned formats
	1,
	2,
	3,
	4,

	2,
	4,
	6,
	8,

	// Signed formats
	1,
	2,
	3,
	4,

	2,
	4,
	6,
	8,

	// Float formats
	2,
	4,
	6,
	8,

	4,
	8,
	12,
	16,

	// Signed integer formats
	2,
	4,
	6,
	8,

	4,
	8,
	12,
	16,

	// Unsigned integer formats
	2,
	4,
	6,
	8,

	4,
	8,
	12,
	16,

	// Packed formats
	4,
	4,
	4,
	2,
	2,
	4,

	// Depth formats
	2,
	4,
	4,
	4,

	// Compressed formats 
	1/2, //FORMAT_DXT1     = 51, this has an 8x compression   not sure about this    
	1, //FORMAT_DXT2     = 52, this has a 4x compression
	1, //FORMAT_DXT3     = 53,this has a 4x compression
	1, //FORMAT_DXT4    = 54,this has a 4x compression
	1, //FORMAT_DXT5    = 55,this has a 4x compression

	0, //FORMAT_RG32TL   = 56,


};

