#ifndef COCEAN_H
#define COCEAN_H
#include "../Graphics_Lib/Globals_GraphicsCore.h"
#include "../Graphics_Lib/GraphicsCore.h"
#include "../Common_Game_Objects_Lib/cWorldDefines.h"
#include "../Common_Game_Objects_Lib/Base_Ocean.h"
#define PASSES 8
#define FFT_SIZE (1<<PASSES)

class cAABB;
class cDataPacket;
class cPeer;

class cOcean: public Base_Ocean{
public:

	cOcean();
	virtual ~cOcean() { DeInit(); }
	void Init();
	void DeInit();
	void Draw();
	void Draw1();

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
	size_t NumIndices0, NumIndices1, NumIndices2;

	bool Choppy;
	float LodDist0, LodDist1, LodDist2;
	bool Editing, DrawWireFrame;
	vec4 EditorPos;

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
	
	Graphics::VertexShader VS_Render;
	Graphics::HullShader HS_Test;
	Graphics::DomainShader DS_Test;
	Graphics::GeometryShader GS_QuadWRT;
	Graphics::PixelShader PS_Variance, PS_Init, PS_FFTY, PS_FFTX, PS_Render, PS_SkyMap;

	Graphics::Buffer CBuffer_PS_Init, CBuffer_GS_FFT, CBuffer_PS_Variance, CBuffer_Render;

	Graphics::Texture transmittance, skyIrradiance, variancesFbo, fftbTex, fftaTex, ButterFlyTex, Spectrum12, Spectrum34, InScatter, SkyMap;
	Graphics::Buffer VertexBuffer, IndexBuffer,  InstanceBuffer;
	
	std::vector<vec4> Grids, Drawing_Grids0, Drawing_Grids1, Drawing_Grids2;


};


#endif