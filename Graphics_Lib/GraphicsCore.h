#ifndef GRAPHICS_CORE_H
#define GRAPHICS_CORE_H

#include "Globals_GraphicsCore.h"
#include <string>
#include <fstream>
#include <direct.h>
#include <mutex>
#include <vector>
#include <map>
#include <set>
#include <atomic>
#include "../Utilities/Utilities.h"
#include "../Utilities/My_Timer.h"

namespace Graphics{
	struct Shader_Macro{// this is castable to a D3D10_SHADER_MACRO 
		const char* Name;
		const char* Definition;
	};
	
	class ViewPort{// castable into directx viewport
	public:
		ViewPort() : TopLeftX(0), TopLeftY(0), Width(0), Height(0), MinDepth(0), MaxDepth(1) {}
		float TopLeftX;
		float TopLeftY;
		float Width;
		float Height;
		float MinDepth;
		float MaxDepth;
	};
	namespace Internal{
	
		class OnResizeCB_S{public: OnResizeCB_S(std::function< void(unsigned int, unsigned int)> f, unsigned int i):func(f), id(i){} std::function< void(unsigned int, unsigned int)> func; unsigned int id; };

		extern Primitives CurrentTopology;//=PRIM_TRIANGLE_FAN;// a triangle fan is no longer supported so it will set the first time through
		extern IDXGISwapChain			*SwapChain;
		extern ID3D11RenderTargetView	*BackBufferRTV;
		extern ID3D11Device*    Device;
		extern ID3D11DeviceContext* DeviceContext;
		extern ID3D11RenderTargetView *CurrentColorRT[MAX_RTS];
		extern ID3D11DepthStencilView *CurrentDepthRT;
		extern std::atomic<unsigned int> NextResizeID;
		extern std::vector<OnResizeCB_S> OnResizeCallBacks;
		extern size_t BufferMemory, TrianglesDrawn, DrawCalls, TextureMemory, NumRts;
		extern ViewPort Current_ViewPort;
		extern My_Timer::Frame_Timer FrameTimer;
		inline size_t CalcTriangles( size_t verts ) {
			if( CurrentTopology == PRIM_TRIANGLELIST ) return verts/3;
			else if( CurrentTopology == PRIM_TRIANGLE_STRIP ) return verts-2;
			return 0; /// else line? 
		}
		HRESULT CompileShaderFromMemory(const std::string& strinmem, const std::string& functionname,const std::string& shadermodel, std::vector<char>& tempbuff, D3D10_SHADER_MACRO* defines = 0 );
		HRESULT CompileShaderFromFile(std::string file, std::string functionname, std::string shadermodel, std::vector<char>& tempbuff, D3D10_SHADER_MACRO* defines = 0 );
		void CreateBuffers(int x, int y);
		ID3D11ShaderResourceView *CreateSRV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int32_t firstSlice = -1, const int32_t sliceCount = -1);
		ID3D11RenderTargetView   *CreateRTV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int32_t firstSlice = -1, const int32_t sliceCount = -1);
		ID3D11DepthStencilView   *CreateDSV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int32_t firstSlice = -1, const int32_t sliceCount = -1);

		void AddTexMemory(size_t width, size_t height, size_t slices, DXGI_FORMAT format, bool mips);
		void SubTexMemory(size_t width, size_t height, size_t slices, DXGI_FORMAT format, bool mips);


		void OnResize(int x, int y);
		void Present();
		void Init(int x, int y, HWND wnd);
		void DeInit();
	};

	inline void SetTopology(const Primitives primitives){
		if(primitives!= Internal::CurrentTopology) Internal::DeviceContext->IASetPrimitiveTopology(d3dPrim[primitives]);
		Internal::CurrentTopology = primitives;
	}
	inline void DrawInstanced(const size_t nvertices, const size_t count, const size_t startvert, const size_t startinstance){
		Internal::DrawCalls++;
		Internal::TrianglesDrawn+= count*Internal::CalcTriangles(nvertices);
		Internal::DeviceContext->DrawInstanced((UINT)nvertices, (UINT)count, (UINT)startvert, (UINT)startinstance);
	}
	inline void DrawAuto(){
		Internal::DrawCalls++;
		Internal::DeviceContext->DrawAuto();
	}
	inline void DrawIndexed(const size_t firstIndex, const size_t nIndices){
		Internal::DrawCalls++;
		Internal::TrianglesDrawn+=Internal::CalcTriangles(nIndices);
		Internal::DeviceContext->DrawIndexed((UINT)nIndices, (UINT)firstIndex, 0);
	}
	inline void DrawIndexedInstanced(const size_t firstIndex, const size_t nIndices, const size_t ninstances){
		Internal::DrawCalls++;
		Internal::TrianglesDrawn+=ninstances*Internal::CalcTriangles(nIndices);
		Internal::DeviceContext->DrawIndexedInstanced((UINT)nIndices, (UINT)ninstances, (UINT)firstIndex, 0, 0);
	}
	inline void DrawVertices(size_t nvertices, size_t first){
		Internal::DrawCalls++;
		Internal::TrianglesDrawn+= Internal::CalcTriangles(nvertices);
		Internal::DeviceContext->Draw((UINT)nvertices, (UINT)first);
	}
	class Texture;

	inline void UnBindRenderTargets() { Internal::DeviceContext->OMSetRenderTargets(0, 0, 0); }
	void BindRenderTargets(Texture **colorrts, unsigned int numofrts, const Texture* depthRT =0, unsigned int depthsliceindex = -1, unsigned int *slices =0);
	void ClearRenderTargets(bool clearColor, bool clearDepth, bool clearStencil, float *color= NULL, float depth = 1.0f, unsigned int stencil = 0);
	inline void GetRenderTargets(unsigned int& num, void** arr, ViewPort& vp){ num = (unsigned int)Internal::NumRts; vp = Internal::Current_ViewPort; memcpy(arr, Internal::CurrentColorRT, sizeof(void*)*Internal::NumRts); }
	inline void* GetDepthTarget(){ return (void*)Internal::CurrentDepthRT; }
	inline void BindRenderTargets(unsigned int num, void** arr, void* depth, ViewPort& vp){ Internal::Current_ViewPort= vp; Internal::DeviceContext->RSSetViewports(1, ((D3D11_VIEWPORT*)&vp)); memcpy(Internal::CurrentColorRT, arr, sizeof(void*)*num); Internal::NumRts = (size_t)num; Internal::DeviceContext->OMSetRenderTargets(num, (ID3D11RenderTargetView **)arr, (ID3D11DepthStencilView *)depth);}
	// the callback will  pass width, and height at parameters
	inline unsigned int RegisterForOnResize(std::function<void (unsigned int, unsigned int)> f) { unsigned int i = Internal::NextResizeID++; Internal::OnResizeCallBacks.push_back(Internal::OnResizeCB_S(f, i)); return i;  }
	// any objects that want notification when resize is called, should register themselves. Keep the ID that is returned to UnRegister with..
	void UnRegisterForOnResize(unsigned int);

	class Texture {
	public:

		Texture() : Texture_(0), Srv(0), Rtv(0), Dsv(0), SrvArray(0), RtvArray(0), DsvArray(0), Width(0), Height(0), Depth(0), ArraySize(0), Flags(0), MapType(Not_Set) {}
		~Texture() { Destroy(); }
		Texture(const Texture& cp){ operator=(cp); } 
		Texture& operator=(const Texture& rhs);

		void Save_To_Disk(std::string fname){// file name should contain the path as well
			fname = StripFileExtention(fname);
			if(!FileExists(fname + ".dds")){
				HR(D3DX11SaveTextureToFileA(Internal::DeviceContext, Texture_ , D3DX11_IFF_DDS, (fname + ".dds").c_str() ));// save the file to disk....
			}
		}
		void* Map(MapAccess access, unsigned int subresourceindex=0){
			D3D11_MAPPED_SUBRESOURCE d;
			HR(Internal::DeviceContext->Map(Texture_, subresourceindex, D3D11_MAP(access), 0, &d));
			return d.pData;
		}
		void UnMap(unsigned int subresourceindex=0){ Internal::DeviceContext->Unmap(Texture_, subresourceindex); }
		void Destroy();
		//Update should be used to update the whole texture.
		void Update(void* data, size_t size, size_t stride, int mipslice =0, int arrayslice=0){// size is the total number of pixels in the texture
			int miplevels = static_cast<int>(powf(static_cast<float>(max(Width, Height)), 1.0f/3.0f ));// number of mips generated The smallest size texture dx will generate is a 2x2, not a 1x1
			assert(DXGI_FormatStride[TexFormat]*Width*Height == size*stride);
			Internal::DeviceContext->UpdateSubresource(Texture_, D3D11CalcSubresource(mipslice, arrayslice, miplevels), 0, data, (UINT)(stride*Width), 0);
		}
		static void Copy(Texture& dst, const Texture& src){ Internal::DeviceContext->CopyResource(dst.Texture_, src.Texture_); }

		bool Create(void* data, unsigned int width, unsigned int height, unsigned int depth, const FORMAT format = FORMAT_RGBA8);
		bool Create(void* data, unsigned int width, unsigned int height, unsigned int depth, BufferAccess bufferAccess, CpuAccess cpuaccess,ResourceFlags flags, const FORMAT format);
		bool Create(std::string fname);
		bool Create(ID3D11Resource *resource,  unsigned int flags=0);
		bool CreateRenderTarget(const int32_t width, const int32_t height, const int32_t depth=1, const int32_t mipMapCount=1, const int32_t arraySize=1, const FORMAT format = FORMAT_RGBA8, const int32_t msaaSamples = 1, const int32_t quality = 0, uint32_t flags = NO_TEX_FLAGS);
		bool CreateRenderDepth(const int32_t width, const int32_t height, const int32_t arraySize = 1, const FORMAT format = FORMAT_D24S8, const int32_t msaaSamples = 1, unsigned int flags = 0);
		bool CreateArray(const std::string* files, const unsigned int& numfiles, const unsigned int& texwidth, BufferAccess bufferAccess = DEFAULT, CpuAccess cpuaccess = CPU_NONE, ResourceFlags flags =NO_FLAG, const FORMAT format = FORMAT_RGBA32F);
		bool CreateArray(const unsigned int& numfiles, const unsigned int& texwidth, BufferAccess bufferAccess = DEFAULT, CpuAccess cpuaccess = CPU_NONE,ResourceFlags flags =NO_FLAG, const FORMAT format = FORMAT_RGBA32F);

		void ClearRenderTargets(float *color= NULL);
		void ClearDepthTargets(bool clearDepth, bool clearStencil, float depth = 1.0f, unsigned int stencil = 0);
		void Resize(const int width, const int height, const int depth =1, const int mipMapCount =1, const int arraySize =1 ,  const int32_t msaaSamples =1, const int32_t quality =0);
		void GenerateMips() { Internal::DeviceContext->GenerateMips( Srv); }  
		static void GenerateNormalBumpFromBump(std::string bumfile, std::string tofile);

		template<class T>static void GenerateNormalBumpFromBump(std::vector<T>& bumps, const size_t width, std::vector<int>& normalbumpmap){//this is for raw bump maps.... 
			normalbumpmap.resize(width*width);
			// first do the interior of the texture.. the exterior needs to wrap.. so i put it into seperate loops, then to the corners last  ugly, but the only way
			for(unsigned int i = 1; i < width-1; ++i){
				for(unsigned int j = 1; j < width-1; ++j){// t is the top height, b is the bottom, l = left, r = right.. got it ? dont access the center pixel . . .
					float t = bumps[(i-1)*width + j];
					float b = bumps[(i+1)*width + j];
					float l = bumps[i*width + j - 1];
					float r = bumps[i*width + j + 1];
					normalmap[i*width+j]= GetRGBAFromHeights(t, b, l, r, bumps[i*width + j]);
				}
			}
			for(unsigned int i(1); i < width-1; i++){// DO THE TOP OF THE TEXTURE>>>>.... seperate loops to prevent array overrun
				float t = bumps[(width * (width -1)) + i];// this is the last row
				float b = bumps[width + i];// this is the second row
				float l = bumps[i - 1];// left
				float r = bumps[i + 1];// right
				normalmap[i]= GetRGBAFromHeights(t, b, l, r, bumps[i]);
			}
			for(unsigned int i(1); i < width-1; i++){// DO THE BOTTOM OF THE TEXTURE>>>>.... seperate loops to prevent array overrun
				float t = bumps[(width * (width -2)) + i];// this is the second to the last row
				float b = bumps[i];// this is the first row
				float l = bumps[(width * (width -1)) + i - 1];// this is the last row
				float r = bumps[(width * (width -1)) + i + 1];// this is the last row
				normalmap[(width * (width -1)) + i]= GetRGBAFromHeights(t, b, l, r, bumps[(width * (width -1)) + i ]);
			}
			for(unsigned int i(1); i < width-1; i++){// DO THE Left OF THE TEXTURE>>>>.... seperate loops to prevent array overrun
				float t = bumps[(i-1)*width];// top 
				float b = bumps[(i+1)*width];// bottom
				float l = bumps[(i*width) + width - 1]; // wraps to the other side
				float r = bumps[(i*width) + 1];// the right is fine
				normalmap[i*width]= GetRGBAFromHeights(t, b, l, r, bumps[i*width]);
			}
			for(unsigned int i(1); i < width-1; i++){// DO THE RIGHT OF THE TEXTURE>>>>.... seperate loops to prevent array overrun
				float t = bumps[(i-1)*width + width - 1];// top 
				float b = bumps[(i+1)*width + width - 1];// bottom
				float l = bumps[(i*width) + width - 2]; // left is fine this loop
				float r = bumps[i*width-1];// the right is fine
				normalmap[(i*width) + width - 1]= GetRGBAFromHeights(t, b, l, r, bumps[(i*width) + width - 1]);
			}
			// now, do the corners
			float t = bumps[width * (width -1)];// bottom left
			float b = bumps[0];// top left corner
			float l = bumps[width]; // top right corner
			float r = bumps[width * (width -1) + width -1];// bottom right
			normalmap[0]= GetRGBAFromHeights(t, bumps[width + i], l, r, bumps[0]);// this is the top left corner
			normalmap[width * (width -1)]= GetRGBAFromHeights(bumps[width * (width -2)], b, l, r, bumps[width * (width -1)] );// bottom left
			normalmap[width -1]= GetRGBAFromHeights(r, bumps[width*2-1], bumps[width-1], b, bumps[width]);// top right 
			normalmap[width * (width -1) + width -1]= GetRGBAFromHeights(bumps[(width-2)*width + width - 1], l, bumps[(width*(width-1)) + width - 2], t, bumps[width * (width -1) + width -1]);// bottom right
		}
		static bool ValidBump(std::string& f){
			D3DX11_IMAGE_INFO info;
			HR(D3DX11GetImageInfoFromFileA(f.c_str(), 0, &info, 0));
			return (info.Format == DXGI_FORMAT_R8_UNORM) | (info.Format == DXGI_FORMAT_R16_UNORM) | (info.Format ==DXGI_FORMAT_R16_FLOAT) | (info.Format ==DXGI_FORMAT_R8G8B8A8_UNORM);
		}	
		bool Empty(){ return Texture_==0;}
		void Set_MapType(Map_Type t){ MapType = t;}
		Map_Type Get_MapType(){ return MapType;}

		static std::map<std::string, Texture*> TextureMap;// all textures are stored here
		Map_Type MapType;
		ID3D11Resource *Texture_;
		ID3D11ShaderResourceView *Srv;
		ID3D11RenderTargetView   *Rtv;
		ID3D11DepthStencilView   *Dsv;
		ID3D11ShaderResourceView **SrvArray;
		ID3D11RenderTargetView   **RtvArray;
		ID3D11DepthStencilView   **DsvArray;
		DXGI_FORMAT TexFormat;
		DXGI_FORMAT SrvFormat;
		DXGI_FORMAT RtvFormat;
		DXGI_FORMAT DsvFormat;
		size_t Width, Height, Depth;
		int32_t ArraySize;
		unsigned int Flags;
		std::string FileName;
	};	
	class SamplerState:public NoCopy {
	public:

		SamplerState() : Sampler(0) {}
		~SamplerState() { Destroy(); }
		void Create(Filter filter, unsigned int maxanisotropy =16, AddressMode u= CLAMP,  AddressMode v= CLAMP,  AddressMode w = CLAMP, float lodbias = 0.0f, Comparison comp = NEVER, float* bordercolor =0, float minlod = -FLT_MAX, float maxlod = FLT_MAX);
		void Destroy(){ RELEASECOM(Sampler); }

		ID3D11SamplerState *Sampler;

	};
	struct BOX{
		UINT left;
		UINT top;
		UINT front;
		UINT right;
		UINT bottom;
		UINT back;
	};
	class Buffer{
	public:

		Buffer(): Data(0), Stride(0), Size(0), Srv(0), Type(VERTEX_BUFFER) {}
		~Buffer() { RELEASECOM(Data); RELEASECOM(Srv); Internal::BufferMemory-=Stride*Size; };
		Buffer(const Buffer& obj){ operator=(obj); }
		Buffer& operator=(const Buffer& obj){
			Data = obj.Data;
			if(Data != 0) Data->AddRef();
			Stride= obj.Stride;
			Size= obj.Size;
			Srv = obj.Srv;
			Type = obj.Type;
			if(Srv != 0) Srv->AddRef();
			return *this;
		}

		static void Copy(Buffer& dst, const Buffer& src){ Internal::DeviceContext->CopyResource(dst.Data, src.Data); }

		void Create(size_t count, size_t stride, BufferType type, BufferAccess bufferAccess = DEFAULT, CpuAccess cpuaccess = CPU_NONE, const void *data=0, ResourceFlags flags =NO_FLAG, const FORMAT format = FORMAT_RGBA32F);
		void Save(std::ofstream& stream);
		void Load(std::ifstream& stream, BufferAccess bufferAccess = DEFAULT, CpuAccess cpuaccess = CPU_NONE, ResourceFlags flags=NO_FLAG, const FORMAT format = FORMAT_RGBA32F );
		void Destroy() {Internal::BufferMemory-=Size*Stride; RELEASECOM(Data); RELEASECOM(Srv); Stride=Size=0; } 

		void Update(void* srcdata, unsigned int subresource=0, BOX* box=0, unsigned int rowpitch=0, unsigned int depthpitch=0){
			Internal::DeviceContext->UpdateSubresource(Data, subresource, (D3D11_BOX*)box, srcdata, rowpitch, depthpitch);
		}
		void* Map(MapAccess access, unsigned int subresourceindex=0){
			D3D11_MAPPED_SUBRESOURCE d;
			HR(Internal::DeviceContext->Map(Data, subresourceindex, D3D11_MAP(access), 0, &d));
			return d.pData;
		}
		void UnMap(unsigned int subresourceindex=0){ 
			Internal::DeviceContext->Unmap(Data, subresourceindex); 
		}
		void BindAsVertexBuffer(unsigned int slot =0) const{
			assert(Data != 0);
			if((Current_Vertex_Buffer == Data) ) return;// allready bound or doesnt exist
			unsigned int stride  = (unsigned int)Stride;
			unsigned int offset  = 0;
			Internal::DeviceContext->IASetVertexBuffers(slot, 1, &Data, &stride, &offset);
			Current_Vertex_Buffer = Data;
		}
		static void UnBindVertexBuffer(){
			ID3D11Buffer *d[1] = {0};
			unsigned int stride  = 0;
			unsigned int offset  = 0;
			Internal::DeviceContext->IASetVertexBuffers(0, 1, d, &stride, &offset);
			Current_Vertex_Buffer = NULL;
		}
		void BindAsIndexBuffer(unsigned int offset =0) const{
			assert(Data != 0);
			if(Current_Index_Buffer == Data) return;// allready bound
			DXGI_FORMAT type = Stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			Internal::DeviceContext->IASetIndexBuffer(Data, type, offset);
			Current_Index_Buffer = Data;
		}
		static void UnBindIndexBuffer(){
			Internal::DeviceContext->IASetIndexBuffer(0, DXGI_FORMAT_R16_UINT, 0);
			Current_Index_Buffer = NULL;
		}
		void BindAsSOBuffer()const{
			assert(Data != 0);
			if(Current_SO_Buffer == Data) return;// allready bound
			unsigned int off[1]= {0};
			Internal::DeviceContext->SOSetTargets(1, &Data, off);
			Current_SO_Buffer = Data;
		}
		static void UnBindSOBuffer(){
			Current_SO_Buffer = NULL;
			unsigned int off[1]= {0};
			ID3D11Buffer *d[1] = {0};
			Internal::DeviceContext->SOSetTargets(1, d, off);
		}
		bool Empty() const{ return Data ==0; }
		ID3D11Buffer *Data;
		ID3D11ShaderResourceView *Srv;
		size_t Stride, Size;
		BufferType Type;

		static ID3D11Buffer *Current_Vertex_Buffer, *Current_Index_Buffer, *Current_SO_Buffer;
	};
	// the following bind vertex function is for binding multiple buffers. if you are only binding one, then use the built in function on the Buffer class
	inline void BindVertexBuffers(Buffer** buffer, unsigned int startslot=0, unsigned int numbuffers=1, unsigned int* offset=0){
		assert(numbuffers <= 8);// no more than 8 buffers bound at a time
		assert(startslot <= 8);// no more than 8 buffers bound at a time
		assert(buffer != 0);
		unsigned int strides [8];
		ID3D11Buffer* ppVertexBuffers[8];
		unsigned int off[8];
		for(unsigned int i=0; i < numbuffers; i++){
			strides[i]=(unsigned int)buffer[i]->Stride;
			ppVertexBuffers[i]=buffer[i]->Data;
			off[i]=0;
		}
		if(offset == 0) offset = off;
		Internal::DeviceContext->IASetVertexBuffers(startslot, numbuffers, ppVertexBuffers, strides, offset);
		Buffer::Current_Vertex_Buffer = 0;// set this to null
	}



	class VertexShader: public NoCopy {
	public:

		VertexShader(): InputLayout(0), Shader(0), Hash_Key(0) { }
		~VertexShader(){ Destroy(); }

		void CompileShaderFromFile(std::string file, std::string functionname = "VS", std::string shadermodel = "vs_4_0",  Shader_Macro* defines = 0);
		void CompileShaderFromMemory(std::string str, std::string functionname = "VS", std::string shadermodel = "vs_4_0", Shader_Macro* defines = 0 );
		void CreateInputLayout(const FormatDesc* formatDesc, const unsigned int nAttribs);

		void SetResource(const Texture* tex, unsigned int slot=0) const;
		void SetResource(const Buffer* tex, unsigned int slot=0) const;
		void SetSampler(const SamplerState& sampler, unsigned int slot=0) const;
		void SetConstantBuffer(const Buffer& buffer, unsigned int slot =0) const;
		void Bind() const ;
		static void UnBind();
		void Destroy();

		ID3D11VertexShader*	Shader;
		ID3D11InputLayout*	InputLayout;
		std::vector<char> CompiledShader;
		size_t Hash_Key;

		static ID3D11VertexShader*	CurrentShader;
		static ID3D11SamplerState* CurrentSampler;
		static unsigned int CurrentSamplerSlot;
		static std::map<size_t, ID3D11VertexShader*>	CachedShader;
		static std::map<size_t, ID3D11InputLayout*>	CachedLayouts;//uses the same key as the shader
	};

	class GeometryShader: public NoCopy {
	public:

		GeometryShader() : Shader(0), Hash_Key(0) { }
		~GeometryShader() { Destroy(); }
		void CompileShaderFromFile(std::string file, std::string functionname = "GS", std::string shadermodel = "gs_4_0",  Shader_Macro* defines = 0);
		void CompileShaderFromFile(std::string file, FormatDesc_SO* fd, unsigned int num, std::string functionname = "GS", std::string shadermodel = "gs_4_0",  Shader_Macro* defines = 0);
		void CompileShaderFromMemory(std::string str, std::string functionname = "GS", std::string shadermodel = "gs_4_0", Shader_Macro* defines = 0 );
		void CompileShaderFromMemory(std::string str, FormatDesc_SO* fd, unsigned int num, std::string functionname = "GS", std::string shadermodel = "gs_4_0", Shader_Macro* defines = 0 );
		void SetResource(const Texture* tex, unsigned int slot=0) const;
		void SetResource(const Buffer* tex, unsigned int slot=0) const;
		void SetSampler(const SamplerState& sampler, unsigned int slot=0) const;
		void SetConstantBuffer(const Buffer& buffer, unsigned int slot =0) const;
		void Bind() const ;
		static void UnBind();
		void Destroy();

		ID3D11GeometryShader*	Shader;
		size_t Hash_Key;

		static ID3D11GeometryShader*	CurrentShader;
		static ID3D11SamplerState* CurrentSampler;
		static unsigned int CurrentSamplerSlot;
		static std::map<size_t, ID3D11GeometryShader*>	CachedShader;
	};

	class HullShader: public NoCopy {
	public:

		HullShader() : Shader(0), Hash_Key(0) { }
		~HullShader() { Destroy(); }
		void CompileShaderFromFile(std::string file, std::string functionname = "HS", std::string shadermodel = "hs_5_0",  Shader_Macro* defines = 0);
		void CompileShaderFromMemory(std::string str, std::string functionname = "HS", std::string shadermodel = "hs_5_0", Shader_Macro* defines = 0 );
		void SetResource(const Texture* tex, unsigned int slot=0) const;
		void SetResource(const Buffer* tex, unsigned int slot=0) const;
		void SetSampler(const SamplerState& sampler, unsigned int slot=0) const;
		void SetConstantBuffer(const Buffer& buffer, unsigned int slot =0) const;
		void Bind() const ;
		static void UnBind() ;
		void Destroy();

		ID3D11HullShader*	Shader;
		size_t Hash_Key;

		static ID3D11HullShader*	CurrentShader;
		static ID3D11SamplerState* CurrentSampler;
		static unsigned int CurrentSamplerSlot;
		static std::map<size_t, ID3D11HullShader*>	CachedShader;
	};
	class DomainShader: public NoCopy {
	public:

		DomainShader() : Shader(0), Hash_Key(0) { }
		~DomainShader() { Destroy(); }
		void CompileShaderFromFile(std::string file, std::string functionname = "DS", std::string shadermodel = "ds_5_0",  Shader_Macro* defines = 0);
		void CompileShaderFromMemory(std::string str, std::string functionname = "DS", std::string shadermodel = "ds_5_0", Shader_Macro* defines = 0 );
		void SetResource(const Texture* tex, unsigned int slot=0) const;
		void SetResource(const Buffer* tex, unsigned int slot=0) const;
		void SetSampler(const SamplerState& sampler, unsigned int slot=0) const;
		void SetConstantBuffer(const Buffer& buffer, unsigned int slot =0) const;
		void Bind() const ;
		static void UnBind() ;
		void Destroy();

		ID3D11DomainShader*	Shader;
		size_t Hash_Key;

		static ID3D11DomainShader*	CurrentShader;
		static ID3D11SamplerState* CurrentSampler;
		static unsigned int CurrentSamplerSlot;
		static std::map<size_t, ID3D11DomainShader*>	CachedShader;
	};

	class PixelShader: public NoCopy {
	public:

		PixelShader() : Shader(0), Hash_Key(0) { }
		~PixelShader(){ Destroy(); }

		void CompileShaderFromFile(std::string file, std::string functionname = "PS", std::string shadermodel = "ps_4_0",  Shader_Macro* defines = 0);
		void CompileShaderFromMemory(std::string str, std::string functionname = "PS", std::string shadermodel = "ps_4_0", Shader_Macro* defines = 0 );
		void SetResource(const Texture* tex, unsigned int slot=0) const;
		void SetResource(const Buffer* tex, unsigned int slot=0) const;
		void SetSampler(const SamplerState& sampler, unsigned int slot=0) const;
		void SetConstantBuffer(const Buffer& buffer, unsigned int slot =0) const;
		void Bind() const;
		static void UnBind();
		void Destroy();

		ID3D11PixelShader*	Shader;
		size_t Hash_Key;

		static ID3D11PixelShader*	CurrentShader;
		static ID3D11SamplerState* CurrentSampler[8];
		static unsigned int CurrentSamplerSlot;
		static std::map<size_t, ID3D11PixelShader*>	CachedShader;
	};



	class DepthState: public NoCopy {
	public:

		DepthState() : State(0) {}
		~DepthState() { Destroy(); }
		void Create(bool enabledepthdest, bool writetodepth = true, Comparison depthcomp = LESS, bool enablestenciltesting = false, DepthStencil_Op frontstencilfail= KEEP, DepthStencil_Op frontdepthfail= KEEP, DepthStencil_Op frontstencilpass= KEEP, Comparison frontstencil_op= ALWAYS,  DepthStencil_Op backstencilfail= KEEP, DepthStencil_Op backdepthfail= KEEP, DepthStencil_Op backstencilpass= KEEP, Comparison backstencil_op= ALWAYS);
		void Destroy() { RELEASECOM(State); }
		void Bind(unsigned int stencilref =0 ) const { 
			if((CurrentState != State) | (stencilref != CurrentStencilRef)){
				Internal::DeviceContext->OMSetDepthStencilState(State, stencilref);
				CurrentState = State;
				CurrentStencilRef = stencilref;
			}
		}
		void UnBind() const {
			if(CurrentState == State){// only unbind if this is the current shader
				Internal::DeviceContext->OMSetDepthStencilState(0, 0);
				CurrentState = 0;
				CurrentStencilRef=0;
			}
		}

		ID3D11DepthStencilState* State;

		static ID3D11DepthStencilState* CurrentState;
		static unsigned int CurrentStencilRef;
	};
	class BlendState: public NoCopy {
	public:

		BlendState() : State(0) {}
		~BlendState() { Destroy(); }
		void Create(bool enableblend , Blend srcblend = ONE , Blend dstblend = ZERO, Blend_Op blendop = OP_ADD, Blend srcblendalpha = ONE, Blend destblendalpha = ZERO, Blend_Op blendopalpha = OP_ADD, ColorMask writemask = ALL, bool independantblend = false,  bool alphatocoverage = false);
		void Destroy() { RELEASECOM(State); }

		void Bind(float* blendfactor=0, unsigned int mask =0xffffffff) const { 
			float blending[4] = {0.0f, 0.0f, 0.0f, 0.0f};
			if(blendfactor != 0) memcpy(blending, blendfactor, sizeof(float)*4);
			if((CurrentState != State) | (CurrentMask != mask) | ( memcmp(blending, CurrentBlendFactors,sizeof(float)*4) != 0)  ){
				Internal::DeviceContext->OMSetBlendState(State, blending, mask);
				CurrentState = State;
				memcpy(CurrentBlendFactors, blending, sizeof(float)*4);
				CurrentMask = mask;
			}
		}
		void UnBind() const {
			if(CurrentState == State){// only unbind if this is the current shader
				Internal::DeviceContext->OMSetBlendState(0, 0, 0);
				CurrentState = 0;
				memset(CurrentBlendFactors, 0, sizeof(float)*4);
				CurrentMask=0;
			}
		}
		ID3D11BlendState* State;
		static ID3D11BlendState* CurrentState;
		static float CurrentBlendFactors[4];
		static unsigned int CurrentMask;

	};
	class RasterizerState: public NoCopy {
	public:

		RasterizerState() : State(0) {}
		~RasterizerState() { Destroy(); }
		void Create(Cull_Mode mode, Fill_Mode fill = SOLID, bool depthclipenable = true, bool frontcounterclock = false, int depthbias =0, float depthbiasclamp= 0.0f , float slopescaleddepthbias = 0.0f, bool scissorenable = false, bool multisamplenable = false, bool antialiasedlineanble = false);
		void Destroy() { RELEASECOM(State); }

		void Bind() const { 
			if((CurrentState != State)){
				Internal::DeviceContext->RSSetState(State);
				CurrentState = State;
			}
		}
		void UnBind() const {
			if((CurrentState == State)){
				Internal::DeviceContext->RSSetState(0);
				CurrentState = 0;
			}
		}
		ID3D11RasterizerState* State;
		static ID3D11RasterizerState* CurrentState;

	};
	class Query : public NoCopy{
	public:
		Query() : Query_(0) {}
		~Query() { Destroy() ;}
		void Destroy() { RELEASECOM(Query_); } 

		void CreateQuery(QUERY querytype){
			D3D11_QUERY_DESC d;
			d.MiscFlags =0;
			d.Query = (D3D11_QUERY)querytype;
			HR(Internal::Device->CreateQuery(&d, &Query_));
		}
		void Begin(){ Internal::DeviceContext->Begin(Query_); }
		void End(){ Internal::DeviceContext->End(Query_); }
		bool Poll(void* data, const size_t numofbytes){
			HRESULT te= Internal::DeviceContext->GetData(Query_, data, static_cast<UINT>(numofbytes), 0);
			HR(te);// test for bad things here
			return ( te == S_OK);
		}

		ID3D11Query *Query_;
	};

	namespace Internal_Components{
		// stuff for an AABB, and a OBB
		extern Buffer VS_BV_Cbuffer0, VS_BV_VB, VS_BV_IB;
		extern VertexShader VS_BV;
		extern PixelShader PS_BV;

		// stuff for an Translator tool
		extern Buffer VS_Trans_VB, VS_Trans_IB;
		extern uint16_t Trans_Ind_Cone_Start, Trans_Ind_Cone_Start_Count, Trans_Ind_Rod_Start, Trans_Ind_Rod_Start_Count;

	}
	namespace Shaders {
		extern VertexShader VS_FullScreenQuad, VS_FullScreenQuadWOne, VS_PreHSPassThrough;
		extern PixelShader PS_NormalBumpConverter, PS_Blur;

	};
	namespace Samplers{
		extern SamplerState Nearest, Linear, BiLinear, TriLinear, Anisotropic;
		extern SamplerState NearestClampUVW, LinearClampUVW, BiLinearClampUVW, TriLinearClampUVW, AnisotropicClampUVW;
	};
	namespace Textures{
		extern Texture RT_Base, RT_Normal, RT_DepthStencil, RT_Depth, RT_BackBufferTexture;
	};
	namespace RasterizerStates{
		extern RasterizerState CullNone, CullBack, CullFront, WireBack, WireFront, WireNoCull, CullNone_NoDepthClip;
	};
	namespace BlendStates{
		extern BlendState Add, SrcAlpha, No_Blend;
	};
	namespace DepthStates{
		extern DepthState DepthTest, StencilSet, StencilTest, StencilTestWDepthSet, StencilSetWDepthTest, StencilTestAndInc, NoDepthTest, NoDepthWrite, DepthWriteNoDepthTest, EqualDepthTest;
	};

	inline void GetBackBufferSize(unsigned int& x, unsigned int& y) { x = (unsigned int)Textures::RT_BackBufferTexture.Width; y = (unsigned int)Textures::RT_BackBufferTexture.Height;   }
	inline void GetBackBufferSize(float& x, float& y) { x = (float)Textures::RT_BackBufferTexture.Width; y = (float)Textures::RT_BackBufferTexture.Height;   }
	/*
		Graphics Utility functions below
	*/

	void CreateAABVBuffers();
	void DestroyAABBBuffers();
	void Draw_AABV(const mat4& view, const mat4& proj, const mat4& world, const vec3& center, const vec3& size_of_each_axis);

	void CreateTrans_ToolBuffers();
	void DestroyTrans_ToolBuffers();
	void Draw_Trans_Tool(const mat4& view, const mat4& proj, const mat4& world);

};
#endif