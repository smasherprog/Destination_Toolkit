#ifndef COCEAN_H
#define COCEAN_H
#include "../Graphics_Lib/Globals_GraphicsCore.h"
#include "../Graphics_Lib/GraphicsCore.h"
#include "Base_Ocean.h"

#define PASSES 8
#define FFT_SIZE (1<<PASSES)

class cAABB;

class Ocean: public Base_Ocean {
public:

	Ocean();
	virtual ~Ocean() { DeInit(); }
	void Init(const vec3* sundir, const vec3* pos, const mat4* vp, const Plane* frustumplanes, float worldsize = 8192.0f, float gridsize= 64.0f);
	void DeInit();
	void Draw(float dt);

	virtual void GridAdded(const vec3& tlpos) { OUTPUT_DEBUG_MSG("adding grid at pos"<<tlpos); Grids.push_back(tlpos); }
	virtual void Add_Ocean_Grid(char* data, unsigned int size, size_t channel);
	void CullObjectsOutside(const cAABB& aabb);// this should be called at regular intervals to ensure the removal of items outside the drawing/staging area

	float spectrum12[FFT_SIZE * FFT_SIZE * 4];
	float spectrum34[FFT_SIZE * FFT_SIZE * 4];
	float Wind; // wind speed in meters per second (at 10m above surface)
	float Omega; // sea state (inverse wave age)
	float hdrExposure;
	float A; // wave amplitude factor (should be one)
	const float cm; // Eq 59
	const float km; // Eq 59

	float TesselationFactor;
	bool Editing, DrawWireFrame;
	vec4 EditorPos;
	const vec3 *SunDir, *Position;
	const mat4 *View_Proj;
	const Plane* Frustum_Planes;

	void SetCalmSeas(){ Wind=3.0; Omega=1.2f; A=1.0f; }
	void SetRoughSeas(){ Wind=15.0; Omega=.6f; A=7.0f; }
	void SetNormalSeas(){ Wind=5.0; Omega=.84f; A=2.0f; }

	void ReloadShaders();
protected:

	void computeButterflyLookupTexture();
	void generateWavesSpectrum();
	float spectrum(float kx, float ky, bool omnispectrum = false);
	float sqr(float x);
	float omega(float k);
	void getSpectrumSample(int i, int j, float lengthScale, float kMin, float *result);
	void generateMesh();
	void computeSlopeVarianceTex();
	void simulateFFTWaves();	


	Graphics::HullShader HS_Test;
	Graphics::DomainShader DS_Test;
	Graphics::GeometryShader GS_QuadWRT;
	Graphics::PixelShader PS_Variance, PS_Init, PS_FFTY, PS_FFTX, PS_Render, PS_SkyMap;

	Graphics::Buffer CBuffer_PS_Init, CBuffer_GS_FFT, CBuffer_PS_Variance, CBuffer_Render;

	Graphics::Texture transmittance, skyIrradiance, variancesFbo, fftbTex, fftaTex, ButterFlyTex, Spectrum12, Spectrum34, InScatter, SkyMap;
	Graphics::Buffer InstanceBuffer;

	std::vector<vec4> Grids, Drawing_Grids;


};


#endif