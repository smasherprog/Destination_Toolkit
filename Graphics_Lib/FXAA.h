#ifndef FXAABLENDER_H
#define FXAABLENDER_H
#include "GraphicsCore.h"


class FXAABlender{
public:

	FXAABlender();
	~FXAABlender();
	void Init();
	void DeInit();

	void Blend(Graphics::Texture& in, Graphics::Texture& out);
	// after setting any quality settings, make sure to call recompile.
	void Compile();// Compile just calls Recompile, 
	void ReCompile(unsigned int width, unsigned int height);

	bool Blending;
	Graphics::PixelShader PS;
	Graphics::Buffer PS_ConstBuffer;

	// It is here now to allow easier tuning.
	// Choose the amount of sub-pixel aliasing removal.
	// This can effect sharpness.
	//   1.00 - upper limit (softer)
	//   0.75 - default amount of filtering
	//   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
	//   0.25 - almost off
	//   0.00 - completely off
	float QualitySubpix;

	// Only used on FXAA Quality.
	// This used to be the FXAA_QUALITY__EDGE_THRESHOLD define.
	// It is here now to allow easier tuning.
	// The minimum amount of local contrast required to apply algorithm.
	//   0.333 - too little (faster)
	//   0.250 - low quality
	//   0.166 - default
	//   0.125 - high quality 
	//   0.063 - overkill (slower)
	//   
	float QualityEdgeThreshold;
	// Only used on FXAA Quality.
	// This used to be the FXAA_QUALITY__EDGE_THRESHOLD_MIN define.
	// It is here now to allow easier tuning.
	// Trims the algorithm from processing darks.
	//   0.0833 - upper limit (default, the start of visible unfiltered edges)
	//   0.0625 - high quality (faster)
	//   0.0312 - visible limit (slower)
	// Special notes when using FXAA_GREEN_AS_LUMA,
	//   Likely want to set this to zero.
	//   As colors that are mostly not-green
	//   will appear very dark in the green channel!
	//   Tune by looking at mostly non-green content,
	//   then start at zero and increase until aliasing is a problem.
	float QualityEdgeThresholdMin;

	// Choose the quality preset.
	// This needs to be compiled into the shader as it effects code.
	// Best option to include multiple presets is to 
	// in each shader define the preset, then include this file.
	// 
	// OPTIONS
	// -----------------------------------------------------------------------
	// 10 to 15 - default medium dither (10=fastest, 15=highest quality)
	// 20 to 29 - less dither, more expensive (20=fastest, 29=highest quality)
	// 39       - no dither, very expensive 
	//
	// NOTES
	// -----------------------------------------------------------------------
	// 12 = slightly faster then FXAA 3.9 and higher edge quality (default)
	// 13 = about same speed as FXAA 3.9 and better than 12
	// 23 = closest to FXAA 3.9 visually and performance wise
	//  _ = the lowest digit is directly related to performance
	// _  = the highest digit is directly related to style
	unsigned int QualityPreset;

	unsigned int onresizeid;
};

#endif