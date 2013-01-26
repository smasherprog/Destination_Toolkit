#include "stdafx.h"
#include "Ocean.h"
#include "../Utilities/MyMath.h"
#include <fstream>

#define N_SLOPE_VARIANCE 10
#define GRID1_SIZE 5488.0f // size in meters (i.e. in spatial domain) of the first grid
#define GRID2_SIZE 392.0f // size in meters (i.e. in spatial domain) of the second grid
#define GRID3_SIZE 28.0f // size in meters (i.e. in spatial domain) of the third grid
#define GRID4_SIZE 2.0f // size in meters (i.e. in spatial domain) of the fourth grid
#define INITWIND 5.0f
#define INITOMEGA .84f
#define OCEAN_GRID_SIZE 67

double time(){
	__int64 time;
	__int64 cpuFrequency;
	QueryPerformanceCounter((LARGE_INTEGER*) &time);
	QueryPerformanceFrequency((LARGE_INTEGER*) &cpuFrequency);
	return time / double(cpuFrequency);
}
Ocean::Ocean(): cm(.23f), km(370.0f){

}

void Ocean::Init(const vec3* sundir, const vec3* pos, const mat4* vp,  const Plane* frustumplanes, float worldsize, float gridsize){
	DeInit();	
	SunDir = sundir;
	Position =pos;
	View_Proj = vp;
	Frustum_Planes = frustumplanes;
	Base_Ocean::Init(worldsize, gridsize);

	Wind = INITWIND; // wind speed in meters per second (at 10m above surface)
	Omega = INITOMEGA; // sea state (inverse wave age)
	A = 1.0f;
	hdrExposure = 0.4f;
	Editing=false;
	EditorPos.zero();
	DrawWireFrame=false;
	TesselationFactor = 50;

	ReloadShaders();

	CBuffer_PS_Init.Create(1, sizeof(vec4), CONSTANT_BUFFER);
	CBuffer_GS_FFT.Create(1, sizeof(vec4)*2, CONSTANT_BUFFER);
	CBuffer_PS_Variance.Create(1, sizeof(vec4)*2, CONSTANT_BUFFER);
	CBuffer_Render.Create(1, sizeof(mat4) + (sizeof(vec4)*2), CONSTANT_BUFFER);

	std::vector<vec4> data(256*64);
	vec4* tempdata=&data[0];

	memset(&data[0], 0, sizeof(vec4)*256*64);
	std::ifstream f(Asset_Dir+ "transmittance.raw", std::ios::binary);
	if(!f) OUTPUT_DEBUG_MSG("Failed to open transmittance.raw");
	for(size_t i(0); i< 256*64; i++){// this loop is needed because directx does not support a three component thingy!!
		f.read(reinterpret_cast<char*>(tempdata++), sizeof(float)*3);// read three
	}
	f.close();
	transmittance.Create(&data[0], 256, 64, 1, FORMAT_RGBA32F);

	tempdata=&data[0];
	memset(&data[0], 0, sizeof(vec4)*16*64);
	f.open(Asset_Dir+ "irradiance.raw", std::ios::binary);
	if(!f) OUTPUT_DEBUG_MSG("Failed to open irradiance.raw");
	for(size_t i(0); i< 16*64; i++){// this loop is needed because directx does not support a three component thingy!!
		f.read(reinterpret_cast<char*>(tempdata++), sizeof(float)*3);// read three
	}
	f.close();
	skyIrradiance.Create(&data[0], 64, 16, 1, FORMAT_RGBA32F);

	int res = 64;
	int nr = res / 2;
	int nv = res * 2;
	int nb = res / 2;
	int na = 8;
	f.open(Asset_Dir+ "inscatter.raw", std::ios::binary);
	data.resize(nr*nv*nb*na*4);
	f.read(reinterpret_cast<char*>(&data[0]), nr*nv*nb*na*4*sizeof(float));
	f.close();

	InScatter.Create(&data[0], na*nb, nv, nr, FORMAT_RGBA32F);

	variancesFbo.CreateRenderTarget(N_SLOPE_VARIANCE, N_SLOPE_VARIANCE, N_SLOPE_VARIANCE, 1, 1, FORMAT_RG16F, 1, 0, RENDER_SLICES);

	fftaTex.CreateRenderTarget(FFT_SIZE, FFT_SIZE, 1, 0, 5, FORMAT_RGBA16F, 1, 0, RENDER_SLICES | USE_MIPGEN);
	fftbTex.CreateRenderTarget(FFT_SIZE, FFT_SIZE, 1, 1, 5, FORMAT_RGBA16F, 1, 0, RENDER_SLICES);
	SkyMap.CreateRenderTarget(512, 512, 1, 0, 1, FORMAT_RGBA16F, 1, 0, USE_MIPGEN);

	computeButterflyLookupTexture();
	generateWavesSpectrum();
	computeSlopeVarianceTex();

}

void Ocean::DeInit(){

	HS_Test.Destroy();
	GS_QuadWRT.Destroy();
	PS_Variance.Destroy();
	PS_Init.Destroy();
	PS_FFTY.Destroy();
	PS_FFTX.Destroy();
	PS_Render.Destroy();

	CBuffer_PS_Init.Destroy();
	CBuffer_GS_FFT.Destroy();
	CBuffer_PS_Variance.Destroy();
	CBuffer_Render.Destroy();

	transmittance.Destroy();
	skyIrradiance.Destroy();
	variancesFbo.Destroy();
	fftbTex.Destroy();
	fftaTex.Destroy();
	ButterFlyTex.Destroy();
	Spectrum12.Destroy();
	Spectrum34.Destroy();
	InScatter.Destroy();
	SkyMap.Destroy();
	InstanceBuffer.Destroy();
	Base_Ocean::DeInit();

}
void Ocean::ReloadShaders(){
	PS_Variance.CompileShaderFromFile("Ocean_Variance.fx", "VariancePS");
	PS_Init.CompileShaderFromFile("Ocean_init.fx", "InitPS");

	GS_QuadWRT.CompileShaderFromFile("Ocean.fx", "GSQuadWRT");

	PS_FFTY.CompileShaderFromFile("Ocean_fft.fx", "fftyPS");
	PS_FFTX.CompileShaderFromFile("Ocean_fft.fx", "fftxPS");
	PS_SkyMap.CompileShaderFromFile("Ocean.fx", "SkyMapPS");

	PS_Render.CompileShaderFromFile("Ocean_Render.fx", "PSOcean");

	HS_Test.CompileShaderFromFile("Ocean_HSDS.fx", "PatchHS");
	DS_Test.CompileShaderFromFile("Ocean_HSDS.fx", "HeightFieldPatchDS");

}
int bitReverse(int i, int N){
	int j = i;
	int M = N;
	int Sum = 0;
	int W = 1;
	M = M / 2;
	while (M != 0) {
		j = (i & M) > M - 1;
		Sum += j * W;
		W *= 2;
		M = M / 2;
	}
	return Sum;
}
void computeWeight(int N, int k, float &Wr, float &Wi){
	float theta = 2.0f * Pi * static_cast<float>(k) / static_cast<float>(N);
	Wr = cosf(theta);
	Wi = sinf(theta);
}
void Ocean::computeButterflyLookupTexture(){
	float data[FFT_SIZE * PASSES * 4];

	for (int i = 0; i < PASSES; i++) {
		int nBlocks  = (int) powf(2.0, float(PASSES - 1 - i));
		int nHInputs = (int) powf(2.0, float(i));
		for (int j = 0; j < nBlocks; j++) {
			for (int k = 0; k < nHInputs; k++) {
				int i1, i2, j1, j2;
				if (i == 0) {
					i1 = j * nHInputs * 2 + k;
					i2 = j * nHInputs * 2 + nHInputs + k;
					j1 = bitReverse(i1, FFT_SIZE);
					j2 = bitReverse(i2, FFT_SIZE);
				} else {
					i1 = j * nHInputs * 2 + k;
					i2 = j * nHInputs * 2 + nHInputs + k;
					j1 = i1;
					j2 = i2;
				}

				float wr, wi;
				computeWeight(FFT_SIZE, k * nBlocks, wr, wi);

				int offset1 = 4 * (i1 + i * FFT_SIZE);
				data[offset1 + 0] = (j1 + 0.5f) / FFT_SIZE;
				data[offset1 + 1] = (j2 + 0.5f) / FFT_SIZE;
				data[offset1 + 2] = wr;
				data[offset1 + 3] = wi;

				int offset2 = 4 * (i2 + i * FFT_SIZE);
				data[offset2 + 0] = (j1 + 0.5f) / FFT_SIZE;
				data[offset2 + 1] = (j2 + 0.5f) / FFT_SIZE;
				data[offset2 + 2] = -wr;
				data[offset2 + 3] = -wi;
			}
		}
	}

	ButterFlyTex.Create(data, FFT_SIZE, PASSES, 1, FORMAT_RGBA32F);
	//D3DX10SaveTextureToFile( Renderer.Texture[ButterFlyTex.ID].texture , D3DX10_IFF_DDS, L"ButterFlyTex.dds" );
}
float Ocean::sqr(float x){
	return x * x;
}
float Ocean::omega(float k){
	return sqrtf(9.81f * k * (1.0f + sqr(k / km))); // Eq 24
}
// 1/kx and 1/ky in meters
float Ocean::spectrum(float kx, float ky, bool omnispectrum){
	float U10 = Wind;

	// phase speed
	float k = sqrtf(kx * kx + ky * ky);
	float c = omega(k) / k;

	// spectral peak
	float kp = 9.81f * sqr(Omega / U10); // after Eq 3
	float cp = omega(kp) / kp;

	// friction velocity
	float z0 = 3.7e-5f * sqr(U10) / 9.81f * powf(U10 / cp, 0.9f); // Eq 66
	float u_star = 0.41f * U10 / logf(10.0f / z0); // Eq 60

	float Lpm = expf(- 5.0f / 4.0f * sqr(kp / k)); // after Eq 3
	float gamma = Omega < 1.0f ? 1.7f : 1.7f + 6.0f * logf(Omega); // after Eq 3 // log10 or log??
	float sigma = 0.08f * (1.0f + 4.0f / powf(Omega, 3.0f)); // after Eq 3
	float Gamma = exp(-1.0f / (2.0f * sqr(sigma)) * sqr(sqrtf(k / kp) - 1.0f));
	float Jp = powf(gamma, Gamma); // Eq 3
	float Fp = Lpm * Jp * exp(-Omega / sqrtf(10.0f) * (sqrtf(k / kp) - 1.0f)); // Eq 32
	float alphap = 0.006f * sqrtf(Omega); // Eq 34
	float Bl = 0.5f * alphap * cp / c * Fp; // Eq 31

	float alpham = 0.01f * (u_star < cm ? 1.0f + logf(u_star / cm) : 1.0f + 3.0f * logf(u_star / cm)); // Eq 44
	float Fm = exp(-0.25f * sqr(k / km - 1.0f)); // Eq 41
	float Bh = 0.5f * alpham * cm / c * Fm * Lpm; // Eq 40 (fixed)

	if (omnispectrum) {
		return A * (Bl + Bh) / (k * sqr(k)); // Eq 30
	}
	float a0 = logf(2.0f) / 4.0f; 
	float ap = 4.0f; 
	float am = 0.13f * u_star / cm; // Eq 59
	float Delta = tanh(a0 + ap * powf(c / cp, 2.5f) + am * powf(cm / c, 2.5f)); // Eq 57
	float phi = atan2f(ky, kx);
	if (kx < 0.0f) {
		return 0.0f;
	} else {
		Bl *= 2.0f;
		Bh *= 2.0f;
	}

	return A * (Bl + Bh) * (1.0f + Delta * cosf(2.0f * phi)) / (2.0f * Pi * sqr(sqr(k))); // Eq 67
}
long lrandom(long *seed){
	*seed = (*seed * 1103515245 + 12345) & 0x7FFFFFFF;
	return *seed;
}

float frandom(long *seed){
	long r = lrandom(seed) >> (31 - 24);
	return r / (float)(1 << 24);
}

void Ocean::getSpectrumSample(int i, int j, float lengthScale, float kMin, float *result){
	static long seed = 1234;
	float dk = 2.0f * Pi / lengthScale;
	float kx = i * dk;
	float ky = j * dk;
	if (abs(kx) < kMin && abs(ky) < kMin) {
		result[0] = result[1] = 0.0f;
	} else {
		float S = spectrum(kx, ky);
		float h = sqrtf(S / 2.0f) * dk;
		float phi = frandom(&seed) * 2.0f * Pi;

		result[0] = h * cos(phi);
		result[1] = h * sin(phi);
	}
}
void Ocean::generateWavesSpectrum(){
	for (int y = 0; y < FFT_SIZE; ++y) {
		for (int x = 0; x < FFT_SIZE; ++x) {
			int offset = 4 * (x + y * FFT_SIZE);
			int i = x >= FFT_SIZE / 2 ? x - FFT_SIZE : x;
			int j = y >= FFT_SIZE / 2 ? y - FFT_SIZE : y;
			getSpectrumSample(i, j, GRID1_SIZE, Pi / GRID1_SIZE, spectrum12 + offset);
			getSpectrumSample(i, j, GRID2_SIZE, Pi * FFT_SIZE / GRID1_SIZE, spectrum12 + offset + 2);
			getSpectrumSample(i, j, GRID3_SIZE, Pi * FFT_SIZE / GRID2_SIZE, spectrum34 + offset);
			getSpectrumSample(i, j, GRID4_SIZE, Pi * FFT_SIZE / GRID3_SIZE, spectrum34 + offset + 2);
		}
	}

	if(Spectrum34.Empty()) Spectrum34.Create(spectrum34, FFT_SIZE, FFT_SIZE, 1, FORMAT_RGBA32F);
	else Spectrum34.Update(spectrum34, FFT_SIZE*FFT_SIZE, sizeof(float)*4);

	if(Spectrum12.Empty()) Spectrum12.Create(spectrum12, FFT_SIZE, FFT_SIZE, 1,  FORMAT_RGBA32F);
	else Spectrum12.Update(spectrum12, FFT_SIZE*FFT_SIZE, sizeof(float)*4);

}
float getSlopeVariance(float kx, float ky, float *spectrumSample){
	float kSquare = kx * kx + ky * ky;
	float real = spectrumSample[0];
	float img = spectrumSample[1];
	float hSquare = real * real + img * img;
	return kSquare * hSquare * 2.0f;
}
void Ocean::computeSlopeVarianceTex(){
	OUTPUT_DEBUG_MSG("Building Slope Variance Texture");
	// slope variance due to all waves, by integrating over the full spectrum
	float theoreticSlopeVariance = 0.0f;
	float k = 5e-3f;
	while (k < 1e3) {
		float nextK = k * 1.001f;
		theoreticSlopeVariance += k * k * spectrum(k, 0.0f, true) * (nextK - k);
		k = nextK;
	}

	// slope variance due to waves, by integrating over the spectrum part
	// that is covered by the four nested grids. This can give a smaller result
	// than the theoretic total slope variance, because the higher frequencies
	// may not be covered by the four nested grid. Hence the difference between
	// the two is added as a "delta" slope variance in the "variances" shader,
	// to be sure not to lose the variance due to missing wave frequencies in
	// the four nested grids
	float totalSlopeVariance = 0.0;
	for (int y = 0; y < FFT_SIZE; ++y) {
		for (int x = 0; x < FFT_SIZE; ++x) {
			int offset = 4 * (x + y * FFT_SIZE);
			float i = 2.0f * Pi * static_cast<float>(x >= FFT_SIZE / 2 ? x - FFT_SIZE : x);
			float j = 2.0f * Pi * static_cast<float>(y >= FFT_SIZE / 2 ? y - FFT_SIZE : y);
			totalSlopeVariance += getSlopeVariance(i / GRID1_SIZE, j / GRID1_SIZE, &spectrum12[0] + offset);
			totalSlopeVariance += getSlopeVariance(i / GRID2_SIZE, j / GRID2_SIZE, &spectrum12[0] + offset + 2);
			totalSlopeVariance += getSlopeVariance(i / GRID3_SIZE, j / GRID3_SIZE, &spectrum34[0] + offset);
			totalSlopeVariance += getSlopeVariance(i / GRID4_SIZE, j / GRID4_SIZE, &spectrum34[0] + offset + 2);
		}
	}

	float slopevar= 0.5f * (theoreticSlopeVariance - totalSlopeVariance);
	Graphics::Shaders::VS_FullScreenQuad.Bind();
	PS_Variance.Bind();
	vec4 vars[2];
	vars[0].x = slopevar;
	vars[1].x=0;
	Graphics::GeometryShader::UnBind();
	Graphics::HullShader::UnBind();
	Graphics::DomainShader::UnBind();
	PS_Variance.SetSampler(Graphics::Samplers::Nearest, 0);
	PS_Variance.SetResource(&Spectrum12, 0);
	PS_Variance.SetResource(&Spectrum34, 1);
	Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::BlendStates::No_Blend.Bind();
	Graphics::Texture* tex[1] = {&variancesFbo};
	for (unsigned int layer = 0; layer < N_SLOPE_VARIANCE; ++layer) {
		CBuffer_PS_Variance.Update(&vars);
		PS_Variance.SetConstantBuffer(CBuffer_PS_Variance);
		Graphics::BindRenderTargets(tex, 1, 0, -1, &layer);
		Graphics::DrawVertices(4, 0);
		vars[1].x+=1.0f;
	}

	//D3DX10SaveTextureToFile( Renderer.Texture[variancesFbo.ID].texture , D3DX10_IFF_DDS, L"variancesFbo.dds" );
}

void Ocean::simulateFFTWaves(){
	static float wind = 600;
	static float omega = 567;
	static float LastA = 567;
	if((wind != Wind) | (omega!= Omega) | (LastA != A)) {
		generateWavesSpectrum();
		wind = Wind;
		omega = Omega;
		LastA = A;
	}

	Graphics::Shaders::VS_FullScreenQuad.Bind();
	Graphics::GeometryShader::UnBind();
	Graphics::HullShader::UnBind();
	Graphics::DomainShader::UnBind();
	PS_Init.Bind();

	PS_Init.SetResource(&Spectrum12, 0);
	PS_Init.SetResource(&Spectrum34, 1);
	PS_Init.SetSampler(Graphics::Samplers::Nearest, 0);
	PS_Init.SetSampler(Graphics::Samplers::Linear, 1);
	Graphics::SetTopology(PRIM_TRIANGLE_STRIP);

	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::BlendStates::No_Blend.Bind();

	unsigned int slices[5] = {0, 1, 2, 3, 4 };
	Graphics::Texture* arr[5]={&fftaTex, &fftaTex, &fftaTex, &fftaTex, &fftaTex};
	Graphics::BindRenderTargets(arr, 5, 0, -1, slices);
	vec4 v;
	v.x=static_cast<float>(time());
	CBuffer_PS_Init.Update(&v);
	PS_Init.SetConstantBuffer(CBuffer_PS_Init);
	Graphics::DrawVertices(4, 0);

	PS_FFTX.Bind();

	PS_FFTX.SetSampler(Graphics::Samplers::Linear, 0);
	PS_FFTX.SetSampler(Graphics::Samplers::NearestClampUVW, 1);
	PS_FFTX.SetResource(&ButterFlyTex, 0);
	GS_QuadWRT.Bind();
	struct temp {
		int layers[4];
		vec4 pass;
	};
	temp s;
	s.layers[0] = 5;

	Graphics::Texture* tex[1];
	// There is Ping pong action going on here!!!!! Render to one, render to the next.. kind of accumulation buffer of sorts
	for (int i = 0; i < PASSES; ++i) {
		s.pass.x=static_cast<float>(i + 0.5) / PASSES;

		CBuffer_GS_FFT.Update(&s);
		GS_QuadWRT.SetConstantBuffer(CBuffer_GS_FFT);
		PS_FFTX.SetConstantBuffer(CBuffer_GS_FFT);
		Graphics::UnBindRenderTargets();
		if (i%2 == 0) {
			tex[0] = &fftbTex;
			Graphics::BindRenderTargets(tex, 1);
			PS_FFTX.SetResource(&fftaTex, 1);

		} else {
			tex[0] = &fftaTex;
			Graphics::BindRenderTargets(tex, 1);
			PS_FFTX.SetResource(&fftbTex, 1);
		}

		Graphics::DrawVertices(4, 0);
	}
	PS_FFTX.SetResource((Graphics::Texture*)0, 1);
	PS_FFTY.Bind();

	PS_FFTY.SetSampler(Graphics::Samplers::Linear, 0);
	PS_FFTY.SetSampler(Graphics::Samplers::NearestClampUVW, 1);
	PS_FFTY.SetResource(&ButterFlyTex, 0);

	for (int i = PASSES; i < 2 * PASSES; ++i) {
		s.pass.x=static_cast<float>(i - PASSES + 0.5) / PASSES;
		CBuffer_GS_FFT.Update(&s);
		GS_QuadWRT.SetConstantBuffer(CBuffer_GS_FFT);
		PS_FFTY.SetConstantBuffer(CBuffer_GS_FFT);
		Graphics::UnBindRenderTargets();
		if (i%2 == 0) {
			tex[0] = &fftbTex;
			Graphics::BindRenderTargets(tex, 1);
			PS_FFTY.SetResource(&fftaTex, 1);

		} else {
			tex[0] = &fftaTex;
			Graphics::BindRenderTargets(tex, 1);
			PS_FFTY.SetResource(&fftbTex, 1);
		}
		Graphics::DrawVertices(4, 0);
	}
	Graphics::UnBindRenderTargets();
	fftaTex.GenerateMips();
	//D3DX10SaveTextureToFile( Renderer.Texture[fftaTex.ID].texture , D3DX10_IFF_DDS, L"fftaTex.dds" );
}
void Ocean::Draw(float dt){
	// first go a check to see if there is even any work to be done
	Drawing_Grids.clear();
	for(size_t i =0; i< Grids.size(); i++){
		vec3 a(Grids[i].x+(64.0f)/2.0f, Grids[i].y, Grids[i].z+(64.0f)/2.0f);
		if(InsideFrustum(a, 45.0f, Frustum_Planes) ){ 
			Drawing_Grids.push_back(vec4(Grids[i].x, Grids[i].z, 64.0f, 64.0f));
		}
	}

	size_t sizses = Drawing_Grids.size()+ (unsigned int)Editing;
	if(sizses <=0 ) return;//nothing to see here
	if(sizses > InstanceBuffer.Size){
		InstanceBuffer.Create(sizses, sizeof(vec4), VERTEX_BUFFER, DYNAMIC, CPU_WRITE, 0);
	}
	char* ptr = (char*)InstanceBuffer.Map(MAP_WRITE_DISCARD);
	if(Drawing_Grids.size()>0){
		memcpy(ptr, &Drawing_Grids[0], Drawing_Grids.size()*sizeof(vec4));
		ptr+=Drawing_Grids.size()*sizeof(vec4);
	}
	if(Editing){
		vec4 test = vec4(EditorPos.x, EditorPos. z, 64, 64);
		memcpy(ptr, &test, 1*sizeof(vec4));
	}
	InstanceBuffer.UnMap();
	bool changed = false;
	unsigned int numrts=0;
	void* rts[MAX_RTS] = {0};
	Graphics::ViewPort vp;
	Graphics::GetRenderTargets(numrts, rts, vp);
	void* dv = Graphics::GetDepthTarget();

	static float meow = 500;
	meow += dt;
	if(meow  > 1.0f/30.0f){
		simulateFFTWaves();
		changed = true;
		meow = 0;
	}
	Graphics::Texture* tex[1] = { 0 };
	static vec3 lastsundir = vec3(56, 56, 56);// seed this so the first run succeeeeeds
	struct temp {
		mat4 camviewproj;
		vec4 campos, sunormal;
	};
	temp ts;
	ts.camviewproj = *View_Proj;
	ts.camviewproj.Transpose();
	ts.campos = vec4(*Position, TesselationFactor);
	ts.sunormal = vec4(*SunDir, hdrExposure);
	CBuffer_Render.Update(&ts);
	// other stuff
	if(!CloseTo(lastsundir, *SunDir)){// only update if the sun has changed
		changed = true;
		lastsundir =*SunDir;
		Graphics::DepthStates::NoDepthTest.Bind();
		Graphics::RasterizerStates::CullNone.Bind();
		Graphics::BlendStates::No_Blend.Bind();	
		Graphics::SetTopology(PRIM_TRIANGLE_STRIP);	
		Graphics::Shaders::VS_FullScreenQuadWOne.Bind();
		PS_SkyMap.Bind();
		PS_SkyMap.SetConstantBuffer(CBuffer_Render);

		PS_SkyMap.SetResource(&skyIrradiance, 0);
		PS_SkyMap.SetResource(&InScatter, 1);
		PS_SkyMap.SetSampler(Graphics::Samplers::AnisotropicClampUVW, 0);
		PS_SkyMap.SetSampler(Graphics::Samplers::AnisotropicClampUVW, 1);

		tex[0] = &SkyMap;
		Graphics::BindRenderTargets(tex, 1);
		Graphics::DrawVertices(4, 0);
		Graphics::UnBindRenderTargets();
		SkyMap.GenerateMips();
		//D3DX10SaveTextureToFile( Renderer.Texture[SkyMapTex.ID].texture , D3DX10_IFF_DDS, L"SkyMap.dds" );
	}

	//make sure to bind first so that any bound render targets are unbound
	if(changed) Graphics::BindRenderTargets(numrts,rts,  dv, vp);// reset the render targets if they changed
	Graphics::Shaders::VS_PreHSPassThrough.Bind();
	HS_Test.Bind();
	DS_Test.Bind();
	Graphics::GeometryShader::UnBind();
	PS_Render.Bind();

	HS_Test.SetConstantBuffer(CBuffer_Render);
	DS_Test.SetConstantBuffer(CBuffer_Render);
	PS_Render.SetConstantBuffer(CBuffer_Render);

	DS_Test.SetResource(&fftaTex);
	DS_Test.SetSampler(Graphics::Samplers::Linear, 0);

	PS_Render.SetResource(&transmittance);
	PS_Render.SetResource(&skyIrradiance, 1);
	PS_Render.SetResource(&fftaTex, 2);

	PS_Render.SetResource(&variancesFbo, 3);
	PS_Render.SetResource(&SkyMap, 4);

	PS_Render.SetSampler(Graphics::Samplers::Anisotropic, 0);
	PS_Render.SetSampler(Graphics::Samplers::LinearClampUVW, 1);

	Graphics::DepthStates::DepthTest.Bind();
	if(DrawWireFrame) Graphics::RasterizerStates::WireBack.Bind();
	else Graphics::RasterizerStates::CullBack.Bind();
	Graphics::BlendStates::No_Blend.Bind();	

	Graphics::SetTopology(PRIM_1_CONTROL_POINT);
	InstanceBuffer.BindAsVertexBuffer(0);

	Graphics::DrawVertices(sizses, 0);
	Graphics::HullShader::UnBind();
	Graphics::DomainShader::UnBind();
}


void Ocean::Add_Ocean_Grid(char* data, unsigned int size, size_t channel){
	if(size != sizeof(float)*2) return;// ignore bad packets 
	OUTPUT_DEBUG_MSG("received Conf_Ocean_Grid");
	float x = *reinterpret_cast<float*>(data);
	data+=sizeof(float);
	float y = *reinterpret_cast<float*>(data);
	vec3 tlpos(x,0, y);
	GridAdded(tlpos);

}