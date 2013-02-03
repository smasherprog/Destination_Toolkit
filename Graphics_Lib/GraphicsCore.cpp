#include "stdafx.h"
#include "GraphicsCore.h"
#include <fstream>
#include "Shaders.h"
#include "../Utilities/My_Timer.h"
#include "D3Dcompiler.h"


//Internals
std::vector<Graphics::Internal::OnResizeCB_S> Graphics::Internal::OnResizeCallBacks;
Primitives Graphics::Internal::CurrentTopology = PRIM_UNDEFINED;// a PRIM_UNDEFINED is not supported so it will set the first time through
IDXGISwapChain	*Graphics::Internal::SwapChain=0;
ID3D11RenderTargetView* Graphics::Internal::BackBufferRTV = 0;
ID3D11Device* Graphics::Internal::Device = 0;
ID3D11DeviceContext* Graphics::Internal::DeviceContext = 0;
ID3D11RenderTargetView* Graphics::Internal::CurrentColorRT[MAX_RTS] = { 0, 0, 0, 0, 0, 0, 0, 0};
ID3D11DepthStencilView* Graphics::Internal::CurrentDepthRT = 0;
std::atomic<unsigned int> Graphics::Internal::NextResizeID(0);
size_t  Graphics::Internal::BufferMemory(0),  Graphics::Internal::TrianglesDrawn(0), Graphics::Internal::DrawCalls(0),  Graphics::Internal::TextureMemory(0), Graphics::Internal::NumRts(0);
My_Timer::Frame_Timer Graphics::Internal::FrameTimer;

Graphics::ViewPort Graphics::Internal::Current_ViewPort;
//VertexShader
ID3D11VertexShader*	Graphics::VertexShader::CurrentShader=0;
ID3D11SamplerState* Graphics::VertexShader::CurrentSampler=0;
unsigned int Graphics::VertexShader::CurrentSamplerSlot=0;
std::map<size_t, ID3D11VertexShader*>	Graphics::VertexShader::CachedShader;
std::map<size_t, ID3D11InputLayout*>	Graphics::VertexShader::CachedLayouts;//uses the same key as the shader

//Geometry shader
ID3D11GeometryShader*	Graphics::GeometryShader::CurrentShader=0;
ID3D11SamplerState* Graphics::GeometryShader::CurrentSampler=0;
unsigned int Graphics::GeometryShader::CurrentSamplerSlot=0;
std::map<size_t, ID3D11GeometryShader*>	Graphics::GeometryShader::CachedShader;

//Hull Shader
ID3D11HullShader*	Graphics::HullShader::CurrentShader=0;
ID3D11SamplerState* Graphics::HullShader::CurrentSampler=0;
unsigned int Graphics::HullShader::CurrentSamplerSlot=0;
std::map<size_t, ID3D11HullShader*>	Graphics::HullShader::CachedShader;

//Domain Shader
ID3D11DomainShader*	Graphics::DomainShader::CurrentShader=0;
ID3D11SamplerState* Graphics::DomainShader::CurrentSampler=0;
unsigned int Graphics::DomainShader::CurrentSamplerSlot=0;
std::map<size_t, ID3D11DomainShader*>	Graphics::DomainShader::CachedShader;

//pixel shader
ID3D11PixelShader*	Graphics::PixelShader::CurrentShader=0;
ID3D11SamplerState* Graphics::PixelShader::CurrentSampler[8]={0};
std::map<size_t, ID3D11PixelShader*>	Graphics::PixelShader::CachedShader;

ID3D11Buffer *Graphics::Buffer::Current_Vertex_Buffer=0;
ID3D11Buffer *Graphics::Buffer::Current_Index_Buffer=0;
ID3D11Buffer *Graphics::Buffer::Current_SO_Buffer=0;
//Depthstates
ID3D11DepthStencilState* Graphics::DepthState::CurrentState=0;
unsigned int Graphics::DepthState::CurrentStencilRef(0);

//blendstates
ID3D11BlendState* Graphics::BlendState::CurrentState=0;
float Graphics::BlendState::CurrentBlendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f};
unsigned int Graphics::BlendState::CurrentMask=0;

//rasterizerstates
ID3D11RasterizerState* Graphics::RasterizerState::CurrentState=0;

Graphics::VertexShader Graphics::Shaders::VS_FullScreenQuad, Graphics::Shaders::VS_FullScreenQuadWOne, Graphics::Shaders::VS_PreHSPassThrough;
Graphics::PixelShader Graphics::Shaders::PS_NormalBumpConverter, Graphics::Shaders::PS_Blur;

// stuff for an AABB, and a OBB
Graphics::Buffer Graphics::Internal_Components::VS_BV_Cbuffer0,  Graphics::Internal_Components::VS_BV_VB,  Graphics::Internal_Components::VS_BV_IB;
Graphics::VertexShader  Graphics::Internal_Components::VS_BV;
Graphics::PixelShader  Graphics::Internal_Components::PS_BV;

// stuff for an Translator tool
Graphics::Buffer Graphics::Internal_Components::VS_Trans_VB, Graphics::Internal_Components::VS_Trans_IB;



Graphics::SamplerState Graphics::Samplers::Nearest, Graphics::Samplers::Linear, Graphics::Samplers::BiLinear, Graphics::Samplers::TriLinear, Graphics::Samplers::Anisotropic;
Graphics::SamplerState Graphics::Samplers::NearestClampUVW, Graphics::Samplers::LinearClampUVW, Graphics::Samplers::BiLinearClampUVW, Graphics::Samplers::TriLinearClampUVW, Graphics::Samplers::AnisotropicClampUVW;
Graphics::Texture Graphics::Textures::RT_Base, Graphics::Textures::RT_Normal, Graphics::Textures::RT_DepthStencil, Graphics::Textures::RT_Depth, Graphics::Textures::RT_BackBufferTexture;
Graphics::RasterizerState Graphics::RasterizerStates::CullNone, Graphics::RasterizerStates::CullBack, Graphics::RasterizerStates::CullFront, Graphics::RasterizerStates::WireBack, Graphics::RasterizerStates::WireFront, Graphics::RasterizerStates::WireNoCull;
Graphics::RasterizerState Graphics::RasterizerStates::CullNone_NoDepthClip;
Graphics::BlendState Graphics::BlendStates::Add, Graphics::BlendStates::SrcAlpha, Graphics::BlendStates::No_Blend;
Graphics::DepthState Graphics::DepthStates::DepthTest, Graphics::DepthStates::StencilSet, Graphics::DepthStates::StencilTest, Graphics::DepthStates::StencilTestWDepthSet, Graphics::DepthStates::StencilSetWDepthTest, Graphics::DepthStates::StencilTestAndInc, Graphics::DepthStates::NoDepthTest, Graphics::DepthStates::NoDepthWrite, Graphics::DepthStates::DepthWriteNoDepthTest, Graphics::DepthStates::EqualDepthTest;

void Graphics::UnRegisterForOnResize(unsigned int id){
	for(auto i(Internal::OnResizeCallBacks.begin()); i !=Internal::OnResizeCallBacks.end(); i++){
		if(i->id == id){
			Internal::OnResizeCallBacks.erase(i);
			return;
		}
	}
}

void Graphics::Internal::Present(){ 
	SwapChain->Present(0, 0); 
	TrianglesDrawn=DrawCalls=0; 
	// do FPS calculation and Current Framecount
	FrameTimer.Per_Loop();
}


HRESULT Graphics::Internal::CompileShaderFromMemory(const std::string& strinmem, const std::string& functionname,const std::string& shadermodel, std::vector<char>& tempbuff, D3D10_SHADER_MACRO* defines ){
	OUTPUT_DEBUG_MSG("Starting to Compiling Shader from memory: "<<functionname);
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = 0;
#if defined(DEBUG) | defined (_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#else 
	dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
	ID3DBlob* pErrorBlob=0;
	ID3DBlob* ppBlobOut=0;
	//hr = D3DCompile(strinmem.c_str(), strinmem.size(),  NULL, defines, NULL, functionname.c_str(), shadermodel.c_str(), dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	hr = D3DX11CompileFromMemory(strinmem.c_str(), strinmem.size(), NULL, defines, NULL, functionname.c_str(), shadermodel.c_str(),  dwShaderFlags, 0, NULL, &ppBlobOut, &pErrorBlob, NULL );
	if( FAILED(hr) ){
		if( pErrorBlob != NULL ){
			OUTPUT_DEBUG_MSG("Error CompileShaderFromMemory, FunctionName: "<<functionname<<(char*)pErrorBlob->GetBufferPointer() );
		}
		RELEASECOM(pErrorBlob);
		RELEASECOM(ppBlobOut);
		return hr;
	}

	if( pErrorBlob ) pErrorBlob->Release();
	tempbuff.resize(ppBlobOut->GetBufferSize());
	memcpy(&tempbuff[0], ppBlobOut->GetBufferPointer(), ppBlobOut->GetBufferSize());

	RELEASECOM(ppBlobOut);
	OUTPUT_DEBUG_MSG("Finished Compiling Shader from memory: "<<functionname);
	return S_OK;
}
HRESULT Graphics::Internal::CompileShaderFromFile(std::string file, std::string functionname, std::string shadermodel, std::vector<char>& tempbuff, D3D10_SHADER_MACRO* defines){

	OUTPUT_DEBUG_MSG("Starting to Compiling Shader from File: "<<file<<", Function Name: "<<functionname);
	std::string tdir;
	std::string tfile = StripFileExtention(StripPath(file));// now, only the filename without the extension remains

	tfile+=functionname + ".cso";// the ending filename will be the filename with no extension, plus the function name, plus .cso.    .cso is what Microsoft likes to name the compiled shaders
	if(ContainsPath(file)){//if a path is specified, use that
		tdir = file;
	} else {// if not
		tdir = Asset_Dir + "Shaders\\";
	}	
	bool forcerecompile = false;
	//get the date of modification from the shader file to see if a recompile is needed
	WIN32_FILE_ATTRIBUTE_DATA fad, fad1;
	std::string sfsdf =  tdir + StripPath(file);
	bool test = GetFileAttributesEx(sfsdf.c_str(), GetFileExInfoStandard, &fad)!=0;// fx file
	assert(test);// file doesnt exist. This is a fatal programming error. Stop program and fix
	test = GetFileAttributesEx((tdir + tfile).c_str(), GetFileExInfoStandard, &fad1) !=0;// cso file
	if(test){
		if(CompareFileTime(&fad.ftLastWriteTime, &fad1.ftLastWriteTime)==1){
			//the fx file is newer than the cso recompile it
			forcerecompile=true;
		}
	}

	//so passing ../ocean.fx as the file and Render as the function name, you will get      \Shaders\oceanRender.cso  as the final output
	if(!forcerecompile){// if this is not a forced recompile
		std::ifstream f((tdir + tfile).c_str(), std::ios::beg | std::ios::binary);
		if(f){// the file exists, use that instead of recompiling
			OUTPUT_DEBUG_MSG("    Recompile not needed. Using compiled shader instead of compiling");
			f.seekg (0, std::ios::end);
			tempbuff.resize((size_t)f.tellg());// get the filesize
			f.seekg (0, std::ios::beg);
			f.read(&tempbuff[0], tempbuff.size());
			f.close();
			return S_OK;
		}
		// else no file.. goto compiling it
	} 

	HRESULT hr = S_OK;
	DWORD dwShaderFlags = 0;
#if defined(DEBUG) | defined (_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#else 
	dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
	ID3DBlob* pErrorBlob=0;
	ID3DBlob* ppBlobOut=0;

	hr = D3DX11CompileFromFileA((tdir + StripPath(file)).c_str(), defines, NULL,  functionname.c_str(), shadermodel.c_str(),  dwShaderFlags, 0, NULL, &ppBlobOut, &pErrorBlob, NULL );
	if( FAILED(hr) ){
		if( pErrorBlob != NULL ){
			OUTPUT_DEBUG_MSG("Error CompileShaderFromFile, FunctionName: "<<functionname<<(char*)pErrorBlob->GetBufferPointer() );
		}
		RELEASECOM(pErrorBlob);
		RELEASECOM(ppBlobOut);
		return hr;
	}

	if( pErrorBlob ) pErrorBlob->Release();
	tempbuff.resize(ppBlobOut->GetBufferSize());
	memcpy(&tempbuff[0], ppBlobOut->GetBufferPointer(), ppBlobOut->GetBufferSize());
	RELEASECOM(ppBlobOut);
	// save the compiled file
	std::ofstream f((tdir + tfile).c_str(), std::ios::trunc | std::ios::binary);
	if(f){
		f.write(&tempbuff[0], tempbuff.size());
		f.close();
	}else {
		assert(0);// this will error if not taken care of
	}

	OUTPUT_DEBUG_MSG("Finished to Compiling Shader from File: "<<file<<", Function Name: "<<functionname);
	return S_OK;
}


void Graphics::VertexShader::CompileShaderFromFile(std::string file, std::string functionname , std::string shadermodel ,  Shader_Macro* defines){
	Destroy();//make sure this is clean
	std::string search = file + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		HR(Internal::CompileShaderFromFile(file, functionname, shadermodel,CompiledShader, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateVertexShader(&CompiledShader[0],CompiledShader.size(), 0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}
void Graphics::VertexShader::CompileShaderFromMemory(std::string str, std::string functionname , std::string shadermodel, Shader_Macro* defines ){
	Destroy();
	//generate a hash of the shader to use for lookups
	std::string search = str + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		HR(Internal::CompileShaderFromMemory(str, functionname, shadermodel, CompiledShader, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateVertexShader(&CompiledShader[0],CompiledShader.size(), 0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}


void Graphics::VertexShader::CreateInputLayout(const FormatDesc* formatDesc, const unsigned int nAttribs){
	auto j = CachedLayouts.find(Hash_Key);
	if(j!=CachedLayouts.end()){
		InputLayout = j->second;
		unsigned int count1 = InputLayout->AddRef();
		CompiledShader.resize(0);
		return;
	}
	RELEASECOM(InputLayout);
	assert(nAttribs != 0);
	assert(formatDesc != 0);
	static unsigned int counter =0;
	static const DXGI_FORMAT formats[][4] = {
		DXGI_FORMAT_R32_FLOAT,	DXGI_FORMAT_R32G32_FLOAT,	DXGI_FORMAT_R32G32B32_FLOAT,	DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16_FLOAT,	DXGI_FORMAT_R16G16_FLOAT,	DXGI_FORMAT_UNKNOWN,			DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R8_UNORM,	DXGI_FORMAT_R8G8_UNORM,		DXGI_FORMAT_UNKNOWN,			DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8_UINT,	DXGI_FORMAT_R8G8_UINT,		DXGI_FORMAT_UNKNOWN,			DXGI_FORMAT_R8G8B8A8_UINT
	}; 
	static const unsigned int sizes[][4] = {
		sizeof(float),		sizeof(float)*2,	sizeof(float)*3,	sizeof(float)*4, 
		sizeof(uint16_t),	sizeof(uint16_t)*2,	sizeof(uint16_t)*3,	sizeof(uint16_t)*4,
		sizeof(uint8_t),	sizeof(uint8_t)*2,	sizeof(uint8_t)*3,	sizeof(uint8_t)*4,
		sizeof(uint8_t),	sizeof(uint8_t)*2,	sizeof(uint8_t)*3,	sizeof(uint8_t)*4
	};
	static const char *semantics[] = {"COLOR", "POSITION", "TEXCOORD", "NORMAL", "TANGENT", "BINORMAL", "BLENDWEIGHT", "Bone", "BLENDINDICES", "Matrix"};
	unsigned int index[10];
	memset(index, 0, sizeof(index));
	unsigned int vertexsize[MAX_VERTEXSTREAM];
	memset(vertexsize, 0, sizeof(vertexsize));
	std::vector<D3D11_INPUT_ELEMENT_DESC> desc(nAttribs);
	// Fill the vertex element array
	for (unsigned int i = 0; i < nAttribs; i++){
		unsigned int stream = formatDesc[i].Stream;
		unsigned int size = formatDesc[i].Size;
		desc[i].InputSlot = stream;
		desc[i].AlignedByteOffset = (UINT)vertexsize[stream];
		desc[i].SemanticName = semantics[formatDesc[i].Type];
		desc[i].SemanticIndex = index[formatDesc[i].Type]++;
		desc[i].Format = formats[formatDesc[i].Format][size - 1];
		desc[i].InputSlotClass =  formatDesc[i].Instance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		desc[i].InstanceDataStepRate = formatDesc[i].Instance ? 1 : 0;
		vertexsize[stream] += sizes[formatDesc[i].Format][size - 1];
	}

	HR(Internal::Device->CreateInputLayout(&desc[0], nAttribs, &CompiledShader[0], CompiledShader.size(), &InputLayout));
	CompiledShader.resize(0);
	CachedLayouts[Hash_Key] = InputLayout;
}
void Graphics::VertexShader::SetResource(const Texture* tex, unsigned int slot)const  { 
	if(tex == 0) Internal::DeviceContext->VSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->VSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::VertexShader::SetResource(const Buffer* tex, unsigned int slot)const  { 
	if(tex == 0) Internal::DeviceContext->VSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->VSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::VertexShader::SetSampler(const SamplerState& sampler, unsigned int slot)const {
	if((CurrentSamplerSlot != slot) | (sampler.Sampler != CurrentSampler)){
		Internal::DeviceContext->VSSetSamplers(slot, 1, &sampler.Sampler); 
		CurrentSamplerSlot= slot;
		CurrentSampler = sampler.Sampler;
	}
}

void Graphics::VertexShader::SetConstantBuffer(const Buffer& buffer, unsigned int slot) const{
	Internal::DeviceContext->VSSetConstantBuffers(slot, 1, &buffer.Data);
}
void Graphics::VertexShader::Bind() const {
	if(CurrentShader != Shader){
		Internal::DeviceContext->VSSetShader(Shader, 0, 0);
		Internal::DeviceContext->IASetInputLayout(InputLayout);  
		CurrentShader = Shader;
	}

}
void Graphics::VertexShader::UnBind() {
	Internal::DeviceContext->VSSetShader(0, 0, 0);
	CurrentShader = 0;
}
void Graphics::VertexShader::Destroy() { 
	if(InputLayout != 0){
		unsigned int count = InputLayout->Release();
		if(count ==0) {// last reference make sure to remove it from the list
			auto j = CachedLayouts.find(Hash_Key);
			if(j!= CachedLayouts.end()) CachedLayouts.erase(j);
			
		}
	}
	if(Shader!=0){
		unsigned int count = Shader->Release();
		if(count ==0) {// last reference make sure to remove it from the list
			auto k = CachedShader.find(Hash_Key);
			if(k!= CachedShader.end()) CachedShader.erase(k);
			
		}
	}
	Shader =0;
	Hash_Key=0;
	InputLayout =0;
} 




void Graphics::GeometryShader::CompileShaderFromFile(std::string file, std::string functionname , std::string shadermodel,  Shader_Macro* defines){
	Destroy();
	std::string search = file + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromFile(file, functionname, shadermodel, tempbuff, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateGeometryShader(&tempbuff[0], tempbuff.size(),  0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}
void Graphics::GeometryShader::CompileShaderFromFile(std::string file, FormatDesc_SO* fd, unsigned int num, std::string functionname , std::string shadermodel,  Shader_Macro* defines){
	Destroy();
	std::string search = file + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromFile(file, functionname, shadermodel, tempbuff, (D3D10_SHADER_MACRO*)defines));
		UINT ptr[1] = {8*4};
		HR(Internal::Device->CreateGeometryShaderWithStreamOutput(&tempbuff[0], tempbuff.size(), (D3D11_SO_DECLARATION_ENTRY*) fd, num, ptr,1,0 ,0,&Shader));
		CachedShader[Hash_Key] = Shader;
	}

}
void Graphics::GeometryShader::CompileShaderFromMemory(std::string str, std::string functionname , std::string shadermodel, Shader_Macro* defines ){
	Destroy();
	std::string search = str + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromMemory(str, functionname, shadermodel,tempbuff, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateGeometryShader(&tempbuff[0], tempbuff.size(),  0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}

}
void Graphics::GeometryShader::CompileShaderFromMemory(std::string str, FormatDesc_SO* fd, unsigned int num, std::string functionname , std::string shadermodel, Shader_Macro* defines ){
	Destroy();
	std::string search = str + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromMemory(str, functionname, shadermodel, tempbuff, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateGeometryShaderWithStreamOutput(&tempbuff[0], tempbuff.size(), (D3D11_SO_DECLARATION_ENTRY*) fd, num, 0,0,0 ,0,&Shader));
		CachedShader[Hash_Key] = Shader;
	}
}

void Graphics::GeometryShader::SetResource(const Texture* tex, unsigned int slot)const  {
	if(tex == 0) Internal::DeviceContext->GSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->GSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::GeometryShader::SetResource(const Buffer* tex, unsigned int slot) const { 
	if(tex == 0) Internal::DeviceContext->GSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->GSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::GeometryShader::SetSampler(const SamplerState& sampler, unsigned int slot)const {
	if((CurrentSamplerSlot != slot) | (sampler.Sampler != CurrentSampler)){
		Internal::DeviceContext->GSSetSamplers(slot, 1, &sampler.Sampler); 
		CurrentSamplerSlot= slot;
		CurrentSampler = sampler.Sampler;
	}
}
void Graphics::GeometryShader::SetConstantBuffer(const Buffer& buffer, unsigned int slot ) const{
	Internal::DeviceContext->GSSetConstantBuffers(slot, 1, &buffer.Data);
}
void Graphics::GeometryShader::Bind() const { 
	if(CurrentShader != Shader){
		Internal::DeviceContext->GSSetShader(Shader, 0, 0);
		CurrentShader = Shader;
	}
}
void Graphics::GeometryShader::UnBind() {
	Internal::DeviceContext->GSSetShader(0, 0, 0);
	CurrentShader = 0;

}
void Graphics::GeometryShader::Destroy() { 
	if(Shader!=0){
		unsigned int count = Shader->Release();
		if(count ==0) {// last reference make sure to remove it from the list
			auto k = CachedShader.find(Hash_Key);
			if(k!= CachedShader.end()) CachedShader.erase(k);
		}
	}
	Shader =0;
	Hash_Key=0;
} 


void Graphics::HullShader::CompileShaderFromFile(std::string file, std::string functionname , std::string shadermodel,  Shader_Macro* defines){
	Destroy();
	std::string search = file + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromFile(file, functionname, shadermodel, tempbuff, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateHullShader(&tempbuff[0], tempbuff.size(), 0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}
void Graphics::HullShader::CompileShaderFromMemory(std::string str, std::string functionname, std::string shadermodel , Shader_Macro* defines ){
	Destroy();
	std::string search = str + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromMemory(str, functionname, shadermodel, tempbuff, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateHullShader(&tempbuff[0], tempbuff.size(), 0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}
void Graphics::HullShader::SetResource(const Texture* tex, unsigned int slot)const  {
	if(tex == 0) Internal::DeviceContext->HSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->HSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::HullShader::SetResource(const Buffer* tex, unsigned int slot)const  { 
	if(tex == 0) Internal::DeviceContext->HSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->HSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::HullShader::SetSampler(const SamplerState& sampler, unsigned int slot)const {
	if((CurrentSamplerSlot != slot) | (sampler.Sampler != CurrentSampler)){
		Internal::DeviceContext->HSSetSamplers(slot, 1, &sampler.Sampler); 
		CurrentSamplerSlot= slot;
		CurrentSampler = sampler.Sampler;
	}
}
void Graphics::HullShader::SetConstantBuffer(const Buffer& buffer, unsigned int slot) const{
	Internal::DeviceContext->HSSetConstantBuffers(slot, 1, &buffer.Data);
}
void Graphics::HullShader::Bind() const { 
	if(CurrentShader != Shader){
		Internal::DeviceContext->HSSetShader(Shader, 0, 0);
		CurrentShader = Shader;
	}
}
void Graphics::HullShader::UnBind() {
	Internal::DeviceContext->HSSetShader(0, 0, 0);
	CurrentShader = 0;
}
void Graphics::HullShader::Destroy() { 
	if(Shader!=0){
		unsigned int count = Shader->Release();
		if(count ==0) {// last reference make sure to remove it from the list
			auto k = CachedShader.find(Hash_Key);
			if(k!= CachedShader.end()) CachedShader.erase(k);
		}
	}
	Shader =0;
	Hash_Key=0;
} 



void Graphics::DomainShader::CompileShaderFromFile(std::string file, std::string functionname , std::string shadermodel,  Shader_Macro* defines ){
	Destroy();
	std::string search = file + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromFile(file, functionname, shadermodel, tempbuff , (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateDomainShader(&tempbuff[0], tempbuff.size(), 0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}
void Graphics::DomainShader::CompileShaderFromMemory(std::string str, std::string functionname , std::string shadermodel , Shader_Macro* defines ){
	Destroy();
	std::string search = str + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} 
	if(Shader ==0){
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromMemory(str, functionname, shadermodel, tempbuff, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreateDomainShader(&tempbuff[0], tempbuff.size(), 0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}
void Graphics::DomainShader::SetResource(const Texture* tex, unsigned int slot)const  {
	if(tex == 0) Internal::DeviceContext->DSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->DSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::DomainShader::SetResource(const Buffer* tex, unsigned int slot)const  { 
	if(tex == 0) Internal::DeviceContext->DSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->DSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::DomainShader::SetSampler(const SamplerState& sampler, unsigned int slot)const {
	if((CurrentSamplerSlot != slot) | (sampler.Sampler != CurrentSampler)){
		Internal::DeviceContext->DSSetSamplers(slot, 1, &sampler.Sampler); 
		CurrentSamplerSlot= slot;
		CurrentSampler = sampler.Sampler;
	}
}
void Graphics::DomainShader::SetConstantBuffer(const Buffer& buffer, unsigned int slot ) const{
	Internal::DeviceContext->DSSetConstantBuffers(slot, 1, &buffer.Data);
}
void Graphics::DomainShader::Bind() const { 
	if(CurrentShader != Shader){
		Internal::DeviceContext->DSSetShader(Shader, 0, 0);
		CurrentShader = Shader;
	}
}
void Graphics::DomainShader::UnBind() {
	Internal::DeviceContext->DSSetShader(0, 0, 0);
	CurrentShader = 0;
}
void Graphics::DomainShader::Destroy() { 
	if(Shader!=0){
		unsigned int count = Shader->Release();
		if(count ==0) {// last reference make sure to remove it from the list
			auto k = CachedShader.find(Hash_Key);
			if(k!= CachedShader.end()) CachedShader.erase(k);
		}
	}
	Shader =0;
	Hash_Key=0;
} 


void Graphics::PixelShader::CompileShaderFromFile(std::string file, std::string functionname , std::string shadermodel,  Shader_Macro* defines ){
	Destroy();
	std::string search = file + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromFile(file, functionname, shadermodel, tempbuff , (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreatePixelShader(&tempbuff[0], tempbuff.size() , 0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}
void Graphics::PixelShader::CompileShaderFromMemory(std::string str, std::string functionname, std::string shadermodel, Shader_Macro* defines ){
	Destroy();
	std::string search = str + functionname + shadermodel;
	if(defines !=0){
		int i=0;
		do{
			search += defines[i].Name;
			search += defines[i].Definition;
		}while(defines[++i].Name != 0);
	}
	Hash_Key =std::hash<std::string>()(search);
	//search through the cached sahders
	auto k = CachedShader.find(Hash_Key);
	if(k!=CachedShader.end()){
		Shader = k->second;
		Shader->AddRef();
	} else {
		std::vector<char> tempbuff;
		HR(Internal::CompileShaderFromMemory(str, functionname, shadermodel, tempbuff, (D3D10_SHADER_MACRO*)defines));
		HR(Internal::Device->CreatePixelShader(&tempbuff[0], tempbuff.size() , 0, &Shader));
		CachedShader[Hash_Key] = Shader;
	}
}
void Graphics::PixelShader::SetResource(const Texture* tex, unsigned int slot)const  {
	if(tex == 0) Internal::DeviceContext->PSSetShaderResources(slot, 0, 0); 
	else Internal::DeviceContext->PSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::PixelShader::SetResource(const Buffer* tex, unsigned int slot)const  { 
	if(tex == 0) Internal::DeviceContext->PSSetShaderResources(slot, 1, 0); 
	else Internal::DeviceContext->PSSetShaderResources(slot, 1, &tex->Srv); 
}
void Graphics::PixelShader::SetSampler(const SamplerState& sampler, unsigned int slot) const {
	assert(slot<8);
	if(sampler.Sampler != CurrentSampler[slot]){
		Internal::DeviceContext->PSSetSamplers(slot, 1, &sampler.Sampler); 
		CurrentSampler[slot] = sampler.Sampler;
	}
}
void Graphics::PixelShader::SetConstantBuffer(const Buffer& buffer, unsigned int slot) const{
	Internal::DeviceContext->PSSetConstantBuffers(slot, 1, &buffer.Data);
}
void Graphics::PixelShader::Bind() const { 
	if(CurrentShader != Shader){
		Internal::DeviceContext->PSSetShader(Shader, 0, 0);
		CurrentShader = Shader;
	}
}
void Graphics::PixelShader::UnBind() {
	Internal::DeviceContext->PSSetShader(0, 0, 0);
	CurrentShader = 0;
}
void Graphics::PixelShader::Destroy() { 
	if(Shader!=0){
		unsigned int count = Shader->Release();
		if(count ==0) {// last reference make sure to remove it from the list
			auto k = CachedShader.find(Hash_Key);
			if(k!= CachedShader.end()) CachedShader.erase(k);
		}
	}
	Shader =0;
	Hash_Key=0;
} 




void Graphics::BindRenderTargets(Texture **colorrts, unsigned int numofrts, const Texture* depthRT, unsigned int depthsliceindex, unsigned int *slices){

	ID3D11RenderTargetView *rtv[MAX_RTS];
	ID3D11DepthStencilView *dsv;

	if (depthRT == 0) dsv = NULL;
	else if (depthsliceindex == -1) dsv = depthRT->Dsv;
	else  dsv = depthRT->DsvArray[depthsliceindex];
	Internal::CurrentDepthRT = dsv;

	for (unsigned int i = 0; i < numofrts; i++){
		const Texture* rt = colorrts[i];
		int slice = -1;
		if (slices == NULL){
			rtv[i] = colorrts[i]->Rtv;
		} else {
			slice = slices[i];
			rtv[i] = colorrts[i]->RtvArray[slice];
		}
		Internal::CurrentColorRT[i] = rtv[i];
	}
	for (unsigned int i = numofrts; i < MAX_RTS; i++){
		Internal::CurrentColorRT[i]=rtv[i]=NULL;
	}
	Internal::NumRts = numofrts;
	Internal::DeviceContext->OMSetRenderTargets(numofrts, rtv, dsv);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;

	if (numofrts > 0){
		vp.Width  = (float)colorrts[0]->Width;
		vp.Height = (float)colorrts[0]->Height;	
	} else {
		vp.Width  = (float)depthRT->Width;
		vp.Height = (float)depthRT->Height;
	}
	Internal::Current_ViewPort = *((ViewPort*)&vp);
	Internal::DeviceContext->RSSetViewports(1, &vp);
}
void Graphics::ClearRenderTargets(bool clearColor, bool clearDepth, bool clearStencil, float *color, float depth, unsigned int stencil){
	float tempcolor[4] = {0, 0, 0, 0};
	if (clearColor){
		if(color==NULL) color = tempcolor;
		for (int i = 0; i < MAX_RTS; i++){
			if (Internal::CurrentColorRT[i] != 0){
				Internal::DeviceContext->ClearRenderTargetView(Internal::CurrentColorRT[i], color);
			} else break;// done clearing
		}
	}
	if (clearDepth || clearStencil){
		unsigned int clearFlags = 0;
		if (clearDepth)   clearFlags |= D3D11_CLEAR_DEPTH;
		if (clearStencil) clearFlags |= D3D11_CLEAR_STENCIL;
		if (Internal::CurrentDepthRT != 0){
			Internal::DeviceContext->ClearDepthStencilView(Internal::CurrentDepthRT, clearFlags, depth, stencil);
		} 
	}
}


//static declare
std::map<std::string, Graphics::Texture*> Graphics::Texture::TextureMap;

Graphics::Texture& Graphics::Texture::operator=(const Texture& cp){
	Texture_ = cp.Texture_;
	Srv = cp.Srv;
	Rtv = cp.Rtv;
	Dsv = cp.Dsv;
	MapType=cp.MapType;

	if(Texture_ !=0) Texture_->AddRef();
	if(Srv !=0) Srv->AddRef();
	if(Rtv !=0) Rtv->AddRef();
	if(Dsv !=0) Dsv->AddRef();

	TexFormat = cp.TexFormat;
	SrvFormat = cp.SrvFormat;
	RtvFormat = cp.RtvFormat;
	DsvFormat = cp.DsvFormat;

	Width = cp.Width;
	Height = cp.Height;
	Depth = cp.Depth;
	ArraySize = cp.ArraySize;
	Flags = cp.Flags;
	FileName = cp.FileName;

	int sliceCount = (int)((Depth == 1)? ArraySize : Depth);
	if (cp.SrvArray != 0 ){
		SrvArray = new ID3D11ShaderResourceView*[sliceCount];
		for (int i = 0; i < sliceCount; i++){
			SrvArray[i]=cp.SrvArray[i];
			SrvArray[i]->AddRef();
		}

	}
	if (cp.RtvArray!= 0 ){
		RtvArray = new ID3D11RenderTargetView*[sliceCount];
		for (int i = 0; i < sliceCount; i++) {
			RtvArray[i]=cp.RtvArray[i];
			RtvArray[i]->AddRef();
		}

	}
	if (cp.DsvArray!= 0 ){
		DsvArray = new ID3D11DepthStencilView*[sliceCount];
		for (int i = 0; i < sliceCount; i++){
			DsvArray[i]=cp.DsvArray[i];
			DsvArray[i]->AddRef();
		}
	}
	return *this;
}
void Graphics::Texture::Destroy(){ 

	if(Texture_ != 0){
		unsigned int count =Texture_->Release();
		if(count ==0){// the last of the textures have been released.. subtract texture resources
			Internal::SubTexMemory(Width, Height, ArraySize, TexFormat, (Flags & USE_MIPGEN)!=0);
			TextureMap.erase(FileName);// erase from the map too 
		}
		Texture_=0;
	}
	RELEASECOM(Srv);
	RELEASECOM(Rtv);
	RELEASECOM(Dsv);
	int sliceCount = (int)((Depth == 1)? ArraySize : Depth);
	if (SrvArray){
		for (int i = 0; i < sliceCount; i++) SrvArray[i]->Release();	
		DELETE_ARRAY(SrvArray);
	}
	if (RtvArray){
		for (int i = 0; i < sliceCount; i++) RtvArray[i]->Release();
		DELETE_ARRAY(RtvArray);
	}
	if (DsvArray){
		for (int i = 0; i < sliceCount; i++) DsvArray[i]->Release();
		DELETE_ARRAY(DsvArray);
	}
	ArraySize=0;
	Flags=0;
	MapType=Not_Set;
	Width=Height=Depth=0;
	FileName.clear();
}

bool Graphics::Texture::Create(std::string fname){
	Destroy();
	if(!ContainsPath(fname)){// no path.. append the asset directory
		fname = Asset_Dir + fname;
	}
	Texture* te=0;
	auto ret = TextureMap.insert(std::map<std::string, Texture*>::value_type(fname, te));
	if(!ret.second){// allready inserted in the map.. increment the ref counters of the texture by one for this
		Texture* t = ret.first->second;
		Height=t->Height;
		Width=t->Width;
		TexFormat = t->TexFormat;
		FileName = fname;	
		Srv = t->Srv;
		Texture_=t->Texture_;
		Texture_->AddRef();// incrment the ref count
		Srv->AddRef();// same
		return true;
	}

	std::ifstream file(fname.c_str());
	if(!file){
		std::string test ="Cannot Find File for CreateTexture() " + fname;
		MessageBoxA(0, test.c_str(), "Error Opening File", 0);
		return false;
	}
	file.close();
	ID3D11ShaderResourceView *resource(0);
	HR(D3DX11CreateShaderResourceViewFromFileA(Internal::Device, fname.c_str(), 0, 0, &resource, 0));

	D3D11_TEXTURE2D_DESC tex;
	ID3D11Texture2D *t;
	resource->GetResource(reinterpret_cast<ID3D11Resource**>(&t));
	t->GetDesc(&tex);
	Height=tex.Height;
	Width=tex.Width;
	TexFormat = tex.Format;
	FileName = fname;	
	Srv = resource;
	Texture_=t;

	Internal::AddTexMemory(tex.Width, tex.Height, tex.ArraySize, tex.Format, true);
	return true;
}
bool Graphics::Texture::Create(ID3D11Resource *resource, unsigned int flags){
	Destroy();
	Texture_ = resource;
	Srv = Internal::CreateSRV(resource);
	Flags = flags;
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);
	switch (type){
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		D3D11_TEXTURE1D_DESC desc1d;
		((ID3D11Texture1D *) resource)->GetDesc(&desc1d);

		Width  = desc1d.Width;
		Height = 1;
		Depth  = 1;
		TexFormat = desc1d.Format;
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		D3D11_TEXTURE2D_DESC desc2d;
		((ID3D11Texture2D *) resource)->GetDesc(&desc2d);

		Width  = desc2d.Width;
		Height = desc2d.Height;
		Depth  = 1;
		TexFormat = desc2d.Format;
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		D3D11_TEXTURE3D_DESC desc3d;
		((ID3D11Texture3D *) resource)->GetDesc(&desc3d);

		Width  = desc3d.Width;
		Height = desc3d.Height;
		Depth  = desc3d.Depth;
		TexFormat = desc3d.Format;
		break;
	}
	return true;
}
bool Graphics::Texture::Create(void* data, unsigned int width, unsigned int height, unsigned int depth, const FORMAT format) {
	return Create(data, width, height, depth, DEFAULT, CPU_NONE, NO_FLAG, format);
}
bool Graphics::Texture::Create(void* data, const unsigned int width, unsigned int height, unsigned int depth, BufferAccess bufferAccess, CpuAccess cpuaccess, ResourceFlags flags, const FORMAT format){
	assert(width!=0);
	assert(height!=0);
	assert(depth!=0);
	Destroy();
	Width  = width;
	Height = height;
	Depth  = depth;
	ArraySize = static_cast<int32_t>(depth==0 ? 1 : depth);
	TexFormat = formats[format];

	if (width > 1 && height >1 && depth <=1){
		D3D11_TEXTURE2D_DESC desc;
		memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = static_cast<UINT>(width);
		desc.Height= static_cast<UINT>(height);
		desc.Format = TexFormat;
		desc.MipLevels = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = (D3D11_USAGE)bufferAccess;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if(bufferAccess == STAGING) desc.BindFlags=0;
		desc.CPUAccessFlags = cpuaccess;
		desc.ArraySize=1;
		desc.MiscFlags=flags;
		HR(Internal::Device->CreateTexture2D(&desc, NULL, (ID3D11Texture2D **) &Texture_));
		Internal::AddTexMemory(desc.Width, desc.Height, desc.ArraySize, desc.Format, true);
	} else if(height==1 && width>1){
		D3D11_TEXTURE1D_DESC desc;
		memset(&desc, 0, sizeof(D3D11_TEXTURE1D_DESC));
		desc.Width = static_cast<UINT>(width);
		desc.Format = TexFormat;
		desc.MipLevels = 0;
		desc.Usage = (D3D11_USAGE)bufferAccess;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if(bufferAccess == STAGING) desc.BindFlags=0;
		desc.CPUAccessFlags = cpuaccess;
		desc.ArraySize=1;
		desc.MiscFlags=flags;
		HR(Internal::Device->CreateTexture1D(&desc, NULL, (ID3D11Texture1D **) &Texture_));
		Internal::AddTexMemory(desc.Width, 1, desc.ArraySize, desc.Format, true);
	} else if(width > 1 && height >1 && depth> 1){
		D3D11_TEXTURE3D_DESC desc;
		desc.Width  = static_cast<UINT>(width);
		desc.Height = static_cast<UINT>(height);
		desc.Depth  = static_cast<UINT>(depth);
		desc.Format = TexFormat;
		desc.MipLevels = 0;
		desc.Usage =  (D3D11_USAGE)bufferAccess;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = cpuaccess;
		desc.MiscFlags = flags;

		HR(Internal::Device->CreateTexture3D(&desc, NULL, (ID3D11Texture3D **) &Texture_));
		Internal::AddTexMemory(desc.Width, desc.Height, desc.Depth, desc.Format, true);
	}
	if(data != NULL) Internal::DeviceContext->UpdateSubresource(Texture_, 0, 0, data, (UINT)(FormatStride[format]*Width), (UINT)(FormatStride[format]*Depth));
	if(bufferAccess == STAGING) return true;// not more work to do 
	SrvFormat = TexFormat;
	Srv = Internal::CreateSRV(Texture_, SrvFormat);
	return true;
}

bool Graphics::Texture::CreateRenderTarget(const int32_t width, const int32_t height, const int32_t depth, const int32_t mipMapCount, const int32_t arraySize, const FORMAT format, const int32_t msaaSamples, const int32_t quality, uint32_t flags){
	assert(width!=0);
	assert(height!=0);
	assert(depth!=0);
	Destroy();
	Width  = width;
	Height = height;
	Depth  = depth;
	ArraySize = arraySize;
	Flags  = flags;
	TexFormat = formats[format];
	if (depth == 1){
		D3D11_TEXTURE2D_DESC desc;
		desc.Width  = (UINT)Width;
		desc.Height = (UINT)Height;
		desc.Format = TexFormat;
		desc.MipLevels = mipMapCount;
		desc.SampleDesc.Count = msaaSamples;
		desc.SampleDesc.Quality = quality;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;
		if (flags & CUBEMAP){
			desc.ArraySize = 6;
			desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		} else {
			desc.ArraySize = arraySize;
			desc.MiscFlags = 0;
		}
		if (flags & USE_MIPGEN){
			desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		//	cout<<"Creating Render Target";
		Internal::AddTexMemory(desc.Width, desc.Height, desc.ArraySize, desc.Format, (flags&USE_MIPGEN) != 0);
		HR(Internal::Device->CreateTexture2D(&desc, NULL, (ID3D11Texture2D **) &Texture_));
	} else {
		D3D11_TEXTURE3D_DESC desc;
		desc.Width  = (UINT)Width;
		desc.Height = (UINT)Height;
		desc.Depth  = (UINT)Depth;
		desc.Format = TexFormat;
		desc.MipLevels = mipMapCount;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		if (flags & USE_MIPGEN){
			desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		Internal::AddTexMemory(desc.Width, desc.Height, desc.Depth, desc.Format, (flags & USE_MIPGEN) != 0);
		HR(Internal::Device->CreateTexture3D(&desc, NULL, (ID3D11Texture3D **) &Texture_));
	}
	SrvFormat = TexFormat;
	RtvFormat = TexFormat;
	Srv = Internal::CreateSRV(Texture_, SrvFormat);
	Rtv = Internal::CreateRTV(Texture_, RtvFormat);
	int32_t sliceCount = (depth == 1)? arraySize : depth;
	if (flags & SAMPLE_SLICES){
		SrvArray = new ID3D11ShaderResourceView *[sliceCount];
		for (int32_t i = 0; i < sliceCount; i++){
			SrvArray[i] = Internal::CreateSRV(Texture_, SrvFormat, i);
		}
	}
	if (flags & RENDER_SLICES){
		RtvArray = new ID3D11RenderTargetView *[sliceCount];
		for (int32_t i = 0; i < sliceCount; i++){
			RtvArray[i] = Internal::CreateRTV(Texture_, RtvFormat, i);
		}
	}
	return true;
}

bool Graphics::Texture::CreateRenderDepth(const int32_t width, const int32_t height, const int32_t arraySize, const FORMAT format, const int32_t msaaSamples, unsigned int flags){
	Destroy();
	Width  = width;
	Height = height;
	Depth  = 1;
	ArraySize = arraySize;
	Flags  = flags;
	TexFormat = DsvFormat = formats[format];
	D3D11_TEXTURE2D_DESC desc;
	desc.Width  = (UINT)Width;
	desc.Height = (UINT)Height;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = msaaSamples;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	if (flags & CUBEMAP){
		desc.ArraySize = 6;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	} else {
		desc.ArraySize = ArraySize;
		desc.MiscFlags = 0;
	}
	if (flags & SAMPLE_DEPTH){
		switch (DsvFormat){
		case DXGI_FORMAT_D16_UNORM:
			TexFormat = DXGI_FORMAT_R16_TYPELESS;
			SrvFormat = DXGI_FORMAT_R16_UNORM;
			break;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			TexFormat = DXGI_FORMAT_R24G8_TYPELESS;
			SrvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		case DXGI_FORMAT_D32_FLOAT:
			TexFormat = DXGI_FORMAT_R32_TYPELESS;
			SrvFormat = DXGI_FORMAT_R32_FLOAT;
			break;
		}
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}
	desc.Format = TexFormat;
	Internal::AddTexMemory(desc.Width, desc.Height, desc.ArraySize, desc.Format, false);
	HR(Internal::Device->CreateTexture2D(&desc, NULL, (ID3D11Texture2D **) &Texture_));

	Dsv = Internal::CreateDSV(Texture_, DsvFormat);

	if (flags & RENDER_SLICES){
		DsvArray = new ID3D11DepthStencilView *[arraySize];
		for (int32_t i = 0; i < arraySize; i++){
			DsvArray[i] = Internal::CreateDSV(Texture_, DsvFormat, i);
		}
	}
	if (flags & SAMPLE_DEPTH){
		Srv = Internal::CreateSRV(Texture_, SrvFormat);

		if (flags & SAMPLE_SLICES){
			SrvArray = new ID3D11ShaderResourceView *[arraySize];
			for (int32_t i = 0; i < arraySize; i++){
				SrvArray[i] = Internal::CreateSRV(Texture_, SrvFormat, i);
			}
		}
	}
	return true;
}

void Graphics::Texture::ClearRenderTargets(float *color){
	float tempcolor[4] = {0, 0, 0, 0};
	if(color==NULL) color = tempcolor;
	if(Rtv != 0)
		Internal::DeviceContext->ClearRenderTargetView(Rtv, color);
	else if(ArraySize != 0){
		for(int i =0; i< ArraySize; i++){
			Internal::DeviceContext->ClearRenderTargetView(RtvArray[i], color);
		}
	}
}
void Graphics::Texture::ClearDepthTargets(bool clearDepth, bool clearStencil, float depth, unsigned int stencil){
	if (clearDepth | clearStencil){
		unsigned int clearFlags = 0;
		if (clearDepth)   clearFlags |= D3D11_CLEAR_DEPTH;
		if (clearStencil) clearFlags |= D3D11_CLEAR_STENCIL;
		Internal::DeviceContext->ClearDepthStencilView(Dsv, clearFlags, depth, stencil);
	}
}
bool Graphics::Texture::CreateArray(const std::string* files, const unsigned int& numfiles, const unsigned int& texwidth,  BufferAccess bufferAccess, CpuAccess cpuaccess, ResourceFlags flags, const FORMAT format){
	Destroy();
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(D3D11_TEXTURE2D_DESC) );

	Width  = Height = texwidth;
	Depth  = 1;
	ArraySize = numfiles;
	SrvFormat= TexFormat = formats[format];

	for(unsigned int i=0; i<numfiles; i++){
		ID3D11Resource *pRes = NULL;
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		memset(&loadInfo, 0 , sizeof( D3DX11_IMAGE_LOAD_INFO ) );

		loadInfo.Width = 0;
		loadInfo.Height  = 0;
		loadInfo.Depth  = 0;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = 0;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = formats[format];
		loadInfo.Filter = D3DX11_FILTER_NONE;
		loadInfo.MipFilter = D3DX11_FILTER_NONE;
		HRESULT testing;
		testing = D3DX11CreateTextureFromFileA( Internal::Device, files[i].c_str(), &loadInfo, NULL, &pRes, &testing );
		if(testing == D3D11_ERROR_FILE_NOT_FOUND){
			HR(testing = D3DX11CreateTextureFromFileA( Internal::Device, (Asset_Dir + files[i]).c_str(), &loadInfo, NULL, &pRes, NULL));
		} else HR(testing);
		if( pRes ){
			ID3D11Texture2D* pTemp;
			HR(pRes->QueryInterface( __uuidof( ID3D11Texture2D ), (void**)&pTemp ));
			pTemp->GetDesc( &desc );
			if(!Texture_) {
				desc.Usage = (D3D11_USAGE)bufferAccess;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.ArraySize = numfiles;
				desc.CPUAccessFlags = cpuaccess;
				desc.MiscFlags =flags;
				HR(Internal::Device->CreateTexture2D( &desc, NULL, (ID3D11Texture2D **) &Texture_));
			}
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			for(UINT iMip=0; iMip < desc.MipLevels; iMip++){
				Internal::DeviceContext->Map(pTemp, iMip, D3D11_MAP_READ, 0,  &mappedTex2D);
				Internal::DeviceContext->UpdateSubresource( Texture_, 
					D3D11CalcSubresource( iMip, i, desc.MipLevels ),
					NULL,
					mappedTex2D.pData,
					mappedTex2D.RowPitch,
					0 );
				Internal::DeviceContext->Unmap(pTemp, iMip );
			}

			RELEASECOM( pRes );
			RELEASECOM( pTemp );
			//cout<<"\t Filename"<<files[i];
			Internal::AddTexMemory(desc.Width, desc.Height, 1, desc.Format, true);
		} else return false;
	}
	HR(Internal::Device->CreateShaderResourceView(Texture_,0, &Srv));
	SrvArray = new ID3D11ShaderResourceView *[numfiles];
	for (int32_t i = 0; i < static_cast<int32_t>(numfiles); i++){
		SrvArray[i] = Internal::CreateSRV(Texture_, SrvFormat, i);
	}
	return true;
}
bool Graphics::Texture::CreateArray(const unsigned int& numfiles, const unsigned int& texwidth, BufferAccess bufferAccess, CpuAccess cpuaccess, ResourceFlags flags, const FORMAT format){
	Destroy();

	Width  = Height = texwidth;
	Depth  = 1;
	ArraySize = numfiles;
	SrvFormat= TexFormat = formats[format];

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = static_cast<UINT>(texwidth);
	desc.Height= static_cast<UINT>(texwidth);
	desc.Format = TexFormat;
	desc.MipLevels = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = (D3D11_USAGE)bufferAccess;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = cpuaccess;
	desc.ArraySize=numfiles;
	desc.MiscFlags=flags;
	HR(Internal::Device->CreateTexture2D(&desc, NULL, (ID3D11Texture2D **) &Texture_));

	HR(Internal::Device->CreateShaderResourceView(Texture_,0, &Srv));
	SrvArray = new ID3D11ShaderResourceView *[numfiles];
	for (int32_t i = 0; i < static_cast<int32_t>(numfiles); i++){
		SrvArray[i] = Internal::CreateSRV(Texture_, SrvFormat, i);
	}
	return true;
}

void Graphics::Texture::Resize(const int width, const int height, const int depth, const int mipMapCount, const int arraySize,  const int32_t msaaSamples, const int32_t quality ){

	D3D11_RESOURCE_DIMENSION type;
	Texture_->GetType(&type);

	Internal::SubTexMemory(Width, Height, ArraySize, TexFormat, (Flags & USE_MIPGEN)!=0);

	switch (type){

	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		D3D11_TEXTURE1D_DESC desc1d;
		((ID3D11Texture1D *) Texture_)->GetDesc(&desc1d);

		desc1d.Width     = width;
		desc1d.ArraySize = arraySize;

		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		D3D11_TEXTURE2D_DESC desc2d;
		((ID3D11Texture2D *) Texture_)->GetDesc(&desc2d);

		desc2d.Width     = width;
		desc2d.Height    = height;
		desc2d.ArraySize = arraySize;
		desc2d.MipLevels = mipMapCount;
		desc2d.SampleDesc.Count = msaaSamples;
		desc2d.SampleDesc.Quality = quality;

		Texture_->Release();
		HR(Internal::Device->CreateTexture2D(&desc2d, NULL, (ID3D11Texture2D **) &Texture_));
		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		D3D11_TEXTURE3D_DESC desc3d;
		((ID3D11Texture3D *) Texture_)->GetDesc(&desc3d);

		desc3d.Width  = width;
		desc3d.Height = height;
		desc3d.Depth  = depth;
		break;

	default:
		return;
	}

	if (Rtv){
		Rtv->Release();
		Rtv = Internal::CreateRTV(Texture_, RtvFormat);
	}
	if (Dsv){
		Dsv->Release();
		Dsv = Internal::CreateDSV(Texture_, DsvFormat);
	}
	if (Srv){
		Srv->Release();
		Srv = Internal::CreateSRV(Texture_, SrvFormat);
	}
	if (RtvArray){
		for (int i = 0; i < ArraySize; i++){
			RtvArray[i]->Release();
		}
		if (arraySize != ArraySize){
			delete [] RtvArray;
			RtvArray = new ID3D11RenderTargetView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++){
			RtvArray[i] = Internal::CreateRTV(Texture_, RtvFormat, i);
		}
	}
	if (DsvArray){
		for (int i = 0; i < ArraySize; i++){
			DsvArray[i]->Release();
		}
		if (arraySize != ArraySize){
			delete [] DsvArray;
			DsvArray = new ID3D11DepthStencilView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++){
			DsvArray[i] = Internal::CreateDSV(Texture_, DsvFormat, i);
		}
	}
	if (SrvArray){
		for (int i = 0; i < ArraySize; i++){
			SrvArray[i]->Release();
		}
		if (arraySize != ArraySize){
			delete [] SrvArray;
			SrvArray = new ID3D11ShaderResourceView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++){
			SrvArray[i] = Internal::CreateSRV(Texture_, SrvFormat, i);
		}
	}
	Width  = width;
	Height = height;
	Depth  = depth;
	ArraySize = arraySize;
	Internal::AddTexMemory(Width, Height, ArraySize, TexFormat, (Flags & USE_MIPGEN) != 0);
}

void Graphics::Texture::GenerateNormalBumpFromBump(std::string bumfile, std::string tofile){

	ID3D11ShaderResourceView *resource(0), *destnormsrv(0);

	HR(D3DX11CreateShaderResourceViewFromFileA(Internal::Device, bumfile.c_str(), 0, 0, &resource, 0));// load the bump map
	D3D11_TEXTURE2D_DESC tex;
	ID3D11Texture2D *t(0), *destnorm(0);
	resource->GetResource(reinterpret_cast<ID3D11Resource**>(&t));
	t->GetDesc(&tex);

	tex.Format = formats[FORMAT_RGBA8];

	HR(Internal::Device->CreateTexture2D(&tex, NULL, (ID3D11Texture2D **) &destnorm));
	destnormsrv = Internal::CreateSRV(destnorm, formats[FORMAT_RGBA8]);

	HR(D3DX11ComputeNormalMap(Internal::DeviceContext, t, 0, (1 << 0), 10, destnorm));
	t->Release();
	Shaders::VS_FullScreenQuad.Bind();
	Shaders::PS_NormalBumpConverter.Bind();
	Shaders::PS_NormalBumpConverter.SetSampler(Samplers::Anisotropic);

	Internal::DeviceContext->PSSetShaderResources(0, 1, &resource);
	Internal::DeviceContext->PSSetShaderResources(1, 1, &destnormsrv);

	Texture rendertexture;
	rendertexture.CreateRenderTarget( tex.Width, tex.Height, 1, 1, 1, FORMAT_RGBA8, 1, 0, USE_MIPGEN);
	Texture* tex1[1] = { &rendertexture };
	BindRenderTargets(tex1, 1);
	float col[] = { 0.0f,0.0f, 0.0f, 0.0f };
	rendertexture.ClearRenderTargets(col);
	SetTopology(PRIM_TRIANGLE_STRIP);
	RasterizerStates::CullNone.Bind();
	DepthStates::NoDepthTest.Bind();
	BlendStates::No_Blend.Bind();
	DrawVertices(4, 0);
	UnBindRenderTargets();

	destnorm->Release();
	resource->Release();
	destnormsrv->Release();	
	rendertexture.Save_To_Disk(tofile);
}

void Graphics::Buffer::Save(std::ofstream& stream){
	void* ptr=0;
	unsigned int temp=(unsigned int)Stride;
	stream.write((char*)&temp, sizeof(unsigned int));// write the numbe of verts
	temp = (unsigned int)Size;
	stream.write((char*)&temp, sizeof(unsigned int));// write the stride of the vertex buffer
	temp =  (unsigned int)Type;
	stream.write((char*)&temp, sizeof(unsigned int));// write the type of buffer
	Graphics::Buffer v;
	v.Create(Size, temp, Type, STAGING, CPU_READ, 0);
	Graphics::Buffer::Copy(v, *this);
	ptr = v.Map(MAP_READ);	
	stream.write(reinterpret_cast<char*>(ptr), Size*Stride);
	v.UnMap();
}
void Graphics::Buffer::Load(std::ifstream& stream, BufferAccess bufferAccess, CpuAccess cpuaccess, ResourceFlags flags, const FORMAT format ){
	std::vector<char> temp;
	unsigned int size=0;
	stream.read((char*)&size, sizeof(unsigned int));// write the number of verts
	unsigned int stride=0;
	stream.read((char*)&stride, sizeof(unsigned int));// write the stride of the vertex buffer
	unsigned int temptype =  0;
	stream.read((char*)&temptype, sizeof(unsigned int));// write the type of buffer
	Type = (BufferType)temptype;
	temp.resize(stride*size);
	stream.read((char*)&temp[0], stride*size);
	Create(size, stride, Type, bufferAccess, cpuaccess, &temp[0], flags, format);
}
void Graphics::Buffer::Create(size_t count, size_t stride, BufferType type, BufferAccess bufferAccess, CpuAccess cpuaccess, const void *data, ResourceFlags flags, const FORMAT format ){
	Destroy();
	assert(count != 0);
	assert(stride != 0);
	Internal::BufferMemory+=count*stride;
	Stride =stride;
	Size = count;
	Type = type;

	D3D11_BUFFER_DESC desc;
	desc.Usage = (D3D11_USAGE) bufferAccess;
	desc.ByteWidth = (UINT)(count*stride);
	desc.BindFlags =  bufferAccess == STAGING? 0: (D3D11_BIND_FLAG)type;
	desc.CPUAccessFlags = cpuaccess;
	desc.MiscFlags = flags;
	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = data;
	vbData.SysMemPitch = vbData.SysMemSlicePitch = 0;
	HR(Internal::Device->CreateBuffer(&desc, data ? &vbData : NULL, &Data));
	if(type == SHADER_BUFFER){// create a shader resource view too
		Srv = Internal::CreateSRV(Data, formats[format]);
	}
}
void Graphics::SamplerState::Create(Filter filter, unsigned int maxanisotropy, AddressMode u,  AddressMode v,  AddressMode w, float lodbias,  Comparison comp, float* bordercolor, float minlod, float maxlod){
	Destroy();
	D3D11_SAMPLER_DESC desc;
	desc.Filter = Filters[filter];
	desc.ComparisonFunc = (D3D11_COMPARISON_FUNC) comp;
	desc.AddressU = AddressModes[u];
	desc.AddressV = AddressModes[v];
	desc.AddressW = AddressModes[w];
	desc.MipLODBias = lodbias;
	desc.MaxAnisotropy = filter == ANISOTROPIC ? maxanisotropy : 1;
	if(bordercolor !=0 ){
		desc.BorderColor[0] = bordercolor[0];
		desc.BorderColor[1] = bordercolor[1];
		desc.BorderColor[2] = bordercolor[2];
		desc.BorderColor[3] = bordercolor[3];
	} else {
		desc.BorderColor[0] = 0.0f;
		desc.BorderColor[1] = 0.0f;
		desc.BorderColor[2] = 0.0f;
		desc.BorderColor[3] = 0.0f;
	}
	desc.MinLOD = minlod;
	desc.MaxLOD = maxlod;
	HR(Internal::Device->CreateSamplerState(&desc, &Sampler));
}
void Graphics::BlendState::Create(bool enableblend, Blend srcblend, Blend dstblend, Blend_Op blendop, Blend srcblendalpha, Blend destblendalpha, Blend_Op blendopalpha, ColorMask writemask, bool independantblend,  bool alphatocoverage){
	Destroy();
	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = alphatocoverage;
	desc.IndependentBlendEnable =independantblend;
	desc.RenderTarget[0].BlendEnable = enableblend;
	desc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP) blendop;
	desc.RenderTarget[0].SrcBlend = (D3D11_BLEND) srcblend;
	desc.RenderTarget[0].DestBlend = (D3D11_BLEND) dstblend;
	desc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP) blendopalpha;
	desc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND) srcblendalpha;
	desc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND) destblendalpha;
	desc.RenderTarget[0].RenderTargetWriteMask = writemask;
	HR(Internal::Device->CreateBlendState(&desc, &State));

}

void Graphics::DepthState::Create(bool enabledepthdest, bool writetodepth, Comparison depthcomp, bool enablestenciltesting,  DepthStencil_Op frontstencilfail, DepthStencil_Op frontdepthfail, DepthStencil_Op frontstencilpass, Comparison frontstencil_op, DepthStencil_Op backstencilfail, DepthStencil_Op backdepthfail, DepthStencil_Op backstencilpass, Comparison backstencil_op){
	Destroy();

	D3D11_DEPTH_STENCIL_DESC desc;
	memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));

	desc.DepthEnable = (BOOL) enabledepthdest;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = (D3D11_COMPARISON_FUNC) depthcomp;
	desc.StencilEnable = (BOOL) enablestenciltesting;
	desc.StencilReadMask  = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	desc.BackFace. StencilFunc = (D3D11_COMPARISON_FUNC) backstencil_op;
	desc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC) frontstencil_op;

	desc.BackFace. StencilDepthFailOp = (D3D11_STENCIL_OP) backdepthfail;
	desc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP) frontdepthfail;

	desc.BackFace. StencilFailOp = (D3D11_STENCIL_OP) backstencilfail;
	desc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP) frontstencilfail;

	desc.BackFace. StencilPassOp = (D3D11_STENCIL_OP) backstencilpass;
	desc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP) frontstencilpass;

	HR(Internal::Device->CreateDepthStencilState(&desc, &State));
}

void Graphics::RasterizerState::Create(Cull_Mode mode, Fill_Mode fill, bool depthclipenable, bool frontcounterclock, int depthbias, float depthbiasclamp, float slopescaleddepthbias, bool scissorenable, bool multisamplenable, bool antialiasedlineanble){
	Destroy();
	D3D11_RASTERIZER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_RASTERIZER_DESC));
	desc.DepthClipEnable = depthclipenable;
	desc.CullMode = (D3D11_CULL_MODE) mode;
	desc.FillMode = (D3D11_FILL_MODE) fill;
	desc.FrontCounterClockwise = frontcounterclock;
	desc.DepthBias = depthbias;
	desc.DepthBiasClamp = depthbiasclamp;
	desc.SlopeScaledDepthBias = slopescaleddepthbias;
	desc.AntialiasedLineEnable = antialiasedlineanble;
	desc.DepthClipEnable = depthclipenable;
	desc.MultisampleEnable = (BOOL) multisamplenable;
	desc.ScissorEnable = (BOOL) scissorenable;
	HR(Internal::Device->CreateRasterizerState(&desc, &State));

}

void Graphics::Internal::AddTexMemory(size_t width, size_t height, size_t slices, DXGI_FORMAT format, bool mips){
	slices = slices == 0 ? 1: slices;
	float start=static_cast<float>(width*height)*DXGI_FormatStride[format];

	//cout<<"Level 0 size in (MB)"<<start<<" and format index :"<<format<<endl;
	size_t temp = TextureMemory;
	TextureMemory+=static_cast<size_t>(start);
	if(mips != 0 ){
		size_t end = static_cast<size_t>(powf(static_cast<float>(max(width, height)), 1.0f/3.0f ));// number of mips generated
		size_t i(0);
		//cout<<"Gen "<<end<<" Mips for tex (w,h,s) ("<<width<<","<<height<<","<<slices<<")"<<endl;
		do{
			start/=4.0f;
			TextureMemory+=static_cast<size_t>(start*slices);
		} while(++i!=end);
	}
	//cout<<"Internal::AddTexMemory (MB)"<<static_cast<float>(TextureMemory -temp)/1000000.0f<<endl;
}
void Graphics::Internal::SubTexMemory(size_t width, size_t height, size_t slices, DXGI_FORMAT format, bool mips){
	slices = slices ==0 ? 1: slices;
	float start=static_cast<float>(width*height)*DXGI_FormatStride[format];
	//cout<<"Level 0 size in (MB)"<<static_cast<float>(start)/1000000.0f<<" and format index :"<<format<<endl;
	float temp = start;
	if(mips != 0 ){
		size_t end = static_cast<size_t>(powf(static_cast<float>(max(width, height)), 1.0f/3.0f ));// number of mips generated
		size_t i(0);
		//	cout<<"Gen "<<end<<" Mips for tex (w,h,s) ("<<width<<","<<height<<","<<slices<<")"<<endl;
		do{
			start/=4;
			temp+=start*slices;
		} while(++i!=end);
	}
	TextureMemory-=static_cast<size_t>(temp);
}

ID3D11ShaderResourceView* Graphics::Internal::CreateSRV(ID3D11Resource *resource, DXGI_FORMAT format, const int32_t firstSlice, const int32_t sliceCount){
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ID3D11ShaderResourceView *srv(0);
	switch (type){
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		D3D11_TEXTURE1D_DESC desc1d;
		((ID3D11Texture1D *) resource)->GetDesc(&desc1d);

		srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc1d.Format;
		if (desc1d.ArraySize > 1){
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			srvDesc.Texture1DArray.FirstArraySlice = 0;
			srvDesc.Texture1DArray.ArraySize = desc1d.ArraySize;
			srvDesc.Texture1DArray.MostDetailedMip = 0;
			srvDesc.Texture1DArray.MipLevels = desc1d.MipLevels;
		} else {
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MostDetailedMip = 0;
			srvDesc.Texture1D.MipLevels = desc1d.MipLevels;
		}
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		D3D11_TEXTURE2D_DESC desc2d;
		((ID3D11Texture2D *) resource)->GetDesc(&desc2d);

		srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
		if (desc2d.ArraySize > 1){
			if (desc2d.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE){
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MostDetailedMip = 0;
				srvDesc.TextureCube.MipLevels = desc2d.MipLevels;
			} else {
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				if (firstSlice < 0){
					srvDesc.Texture2DArray.FirstArraySlice = 0;
					srvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
				} else {
					srvDesc.Texture2DArray.FirstArraySlice = firstSlice;
					if (sliceCount < 0){
						srvDesc.Texture2DArray.ArraySize = 1;
					} else {
						srvDesc.Texture2DArray.ArraySize = sliceCount;
					}
				}
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.MipLevels = desc2d.MipLevels;
			}
		} else {
			srvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = desc2d.MipLevels;
		}
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		D3D11_TEXTURE3D_DESC desc3d;
		((ID3D11Texture3D *) resource)->GetDesc(&desc3d);
		srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc3d.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MostDetailedMip = 0;
		srvDesc.Texture3D.MipLevels = desc3d.MipLevels;
		break;
	default:
		return NULL;
	}

	HR(Internal::Device->CreateShaderResourceView(resource, &srvDesc, &srv));
	return srv;
}
ID3D11RenderTargetView* Graphics::Internal::CreateRTV(ID3D11Resource *resource, DXGI_FORMAT format, const int32_t firstSlice, const int32_t sliceCount){
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	ID3D11RenderTargetView *rtv;
	switch (type){
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		D3D11_TEXTURE2D_DESC desc2d;
		((ID3D11Texture2D *) resource)->GetDesc(&desc2d);

		rtvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
		if (desc2d.ArraySize > 1){
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			if (firstSlice < 0){
				rtvDesc.Texture2DArray.FirstArraySlice = 0;
				rtvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
			} else {
				rtvDesc.Texture2DArray.FirstArraySlice = firstSlice;
				if (sliceCount < 0){
					rtvDesc.Texture2DArray.ArraySize = 1;
				} else {
					rtvDesc.Texture2DArray.ArraySize = sliceCount;
				}
			}
			rtvDesc.Texture2DArray.MipSlice = 0;
		} else {
			rtvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
		}
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		D3D11_TEXTURE3D_DESC desc3d;
		((ID3D11Texture3D *) resource)->GetDesc(&desc3d);

		rtvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc3d.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		if (firstSlice < 0){
			rtvDesc.Texture3D.FirstWSlice = 0;
			rtvDesc.Texture3D.WSize = desc3d.Depth;
		} else {
			rtvDesc.Texture3D.FirstWSlice = firstSlice;
			if (sliceCount < 0){
				rtvDesc.Texture3D.WSize = 1;
			} else {
				rtvDesc.Texture3D.WSize = sliceCount;
			}
		}
		rtvDesc.Texture3D.MipSlice = 0;
		break;
	default:
		return NULL;
	}
	HR(Internal::Device->CreateRenderTargetView(resource, &rtvDesc, &rtv));
	return rtv;
}
ID3D11DepthStencilView* Graphics::Internal::CreateDSV(ID3D11Resource *resource, DXGI_FORMAT format, const int32_t firstSlice, const int32_t sliceCount){
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	memset(&dsvDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ID3D11DepthStencilView *dsv;
	switch (type){
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		D3D11_TEXTURE2D_DESC desc2d;
		((ID3D11Texture2D *) resource)->GetDesc(&desc2d);

		dsvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
		if (desc2d.ArraySize > 1){
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			if (firstSlice < 0){
				dsvDesc.Texture2DArray.FirstArraySlice = 0;
				dsvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
				dsvDesc.Texture2DArray.MipSlice = 0;
			} else {
				dsvDesc.Texture2DArray.FirstArraySlice = firstSlice;
				if (sliceCount < 0){
					dsvDesc.Texture2DArray.ArraySize = 1;
				} else {
					dsvDesc.Texture2DArray.ArraySize = sliceCount;
				}
				dsvDesc.Texture2DArray.MipSlice = 0;
			}
		} else {
			dsvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
		break;
	default:
		return NULL;
	}
	HR(Internal::Device->CreateDepthStencilView(resource, &dsvDesc, &dsv));
	return dsv;
}
void Graphics::Internal::CreateBuffers(int x, int y){

	ID3D11Texture2D	*backbuffer=0;
	HR(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backbuffer));
	HR(Internal::Device->CreateRenderTargetView(backbuffer, NULL, &BackBufferRTV));
	Textures::RT_BackBufferTexture.Create(backbuffer);
	Textures::RT_BackBufferTexture.Rtv = BackBufferRTV;
	Textures::RT_BackBufferTexture.Width = x;
	Textures::RT_BackBufferTexture.Height =y;
	Internal::DeviceContext->OMSetRenderTargets(1, &BackBufferRTV, 0);
	// Setup the viewport
	D3D11_VIEWPORT viewport;
	viewport.Width  = (float)x;
	viewport.Height = (float)y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	Internal::DeviceContext->RSSetViewports(1, &viewport);
}
void Graphics::Internal::Init(int x, int y, HWND wnd){
	OUTPUT_DEBUG_MSG("Calling Graphics::Internal::Init(int x, int y, HWND wnd)");
	DXGI_SWAP_CHAIN_DESC sd;
	memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferDesc.Width  = x;
	sd.BufferDesc.Height = y;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = wnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;

	D3D_FEATURE_LEVEL found;

	UINT flags =0;
#if defined(DEBUG) | defined (_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HR(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, 0, 0 , D3D11_SDK_VERSION, &sd, &SwapChain, &Internal::Device, &found, &Internal::DeviceContext));

	CreateBuffers(x, y);

	Internal::AddTexMemory(x, y, 1, DXGI_FORMAT_R8G8B8A8_UNORM, false);

	// init the standard shaders
	Shaders::VS_FullScreenQuad.CompileShaderFromMemory(Shader_Defines::FullScreenQuadVS);
	FormatDesc layers[1] = { FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 4, 0) };
	Shaders::VS_FullScreenQuad.CreateInputLayout(layers, 1);
	Shaders::VS_PreHSPassThrough.CompileShaderFromMemory(Shader_Defines::PreHSPassThrough_VS, "PassThroughVS", "vs_5_0");
	Shaders::VS_PreHSPassThrough.CreateInputLayout(layers, 1);
	Shaders::VS_FullScreenQuadWOne.CompileShaderFromMemory(Shader_Defines::FullScreenQuadWOneVS);
	Shaders::VS_FullScreenQuadWOne.CreateInputLayout(layers, 1);
	Shaders::VS_BV.CompileShaderFromMemory(Shader_Defines::BV_VS);
	Shaders::VS_BV.CreateInputLayout(layers, 1);

	CreateAABVBuffers();
	CreateTrans_ToolBuffers();

	Shaders::PS_NormalBumpConverter.CompileShaderFromMemory(Shader_Defines::NormalBumpConverterPS);
	Shaders::PS_Blur.CompileShaderFromMemory(Shader_Defines::Blur_PS);
	Shaders::PS_BV.CompileShaderFromMemory(Shader_Defines::BV_PS);

	//init the standard render targets
	unsigned int width(x), height(y);
	Textures::RT_Base.CreateRenderTarget(width, height, 1, 1, 1, FORMAT_RGBA8);
	Textures::RT_Normal.CreateRenderTarget(width, height, 1, 1, 1, FORMAT_RGBA8S);
	Textures::RT_DepthStencil.CreateRenderDepth (width, height, 1,       FORMAT_D24S8 , 1, SAMPLE_DEPTH);
	Textures::RT_Depth.CreateRenderTarget(width, height, 1, 1, 1, FORMAT_R32F);

	//init the sampler states
	Samplers::Nearest.Create(NEAREST, 1, WRAP, WRAP, WRAP);
	Samplers::Linear.Create(LINEAR, 1, WRAP, WRAP, WRAP);
	Samplers::BiLinear.Create(BILINEAR, 1, WRAP, WRAP, WRAP);
	Samplers::TriLinear.Create(TRILINEAR, 1, WRAP, WRAP, WRAP);
	Samplers::Anisotropic.Create(ANISOTROPIC, 16, WRAP, WRAP, WRAP);

	Samplers::NearestClampUVW.Create(NEAREST, 1);
	Samplers::LinearClampUVW.Create(LINEAR, 1);
	Samplers::BiLinearClampUVW.Create(BILINEAR, 1);
	Samplers::TriLinearClampUVW.Create(TRILINEAR, 1);
	Samplers::AnisotropicClampUVW.Create(ANISOTROPIC, 16);

	//init rasterizerstates
	RasterizerStates::CullNone.Create(CULL_NONE);
	RasterizerStates::CullBack.Create(CULL_BACK);
	RasterizerStates::CullFront.Create(CULL_FRONT);
	RasterizerStates::WireBack.Create(CULL_BACK, WIREFRAME );
	RasterizerStates::WireFront.Create(CULL_FRONT, WIREFRAME );
	RasterizerStates::WireNoCull.Create(CULL_NONE, WIREFRAME );

	RasterizerStates::CullNone_NoDepthClip.Create(CULL_NONE, SOLID, false);
	//init blendstates
	BlendStates::Add.Create(true, ONE, ONE);
	BlendStates::SrcAlpha.Create(true, SRC_ALPHA, INV_SRC_ALPHA, OP_ADD, SRC_ALPHA, INV_SRC_ALPHA);
	//BlendStates::No_Blend.Create(false); // left out on purpose .. no blending is default so the pointer is NULL

	//init depth tests
	DepthStates::DepthTest.Create(true, true);
	DepthStates::StencilSet.Create(false, false, ALWAYS, true, KEEP, KEEP, REPLACE);
	DepthStates::StencilTest.Create(false, false, ALWAYS, true, KEEP, KEEP, KEEP, EQUAL);
	DepthStates::StencilTestWDepthSet.Create(false, true, ALWAYS, true,  KEEP, KEEP, REPLACE, ALWAYS);
	DepthStates::StencilSetWDepthTest.Create(true, true, LESS, true,  KEEP, KEEP, REPLACE);
	DepthStates::StencilTestAndInc.Create(false, false, ALWAYS, true, KEEP, KEEP, INCR, EQUAL);
	DepthStates::NoDepthTest.Create(false, false);
	DepthStates::NoDepthWrite.Create(true,  false);
	DepthStates::DepthWriteNoDepthTest.Create(false, true, ALWAYS);
	DepthStates::EqualDepthTest.Create(true, false, EQUAL);
	OUTPUT_DEBUG_MSG("Finished Graphics::Internal::Init(int x, int y, HWND wnd)");
}
void Graphics::Internal::DeInit(){
	OUTPUT_DEBUG_MSG("Calling Graphics::Internal::DeInit()");

	DeviceContext->OMSetRenderTargets(0, NULL, NULL); 
	DeviceContext->PSSetShader(0,0,0);
	DeviceContext->GSSetShader(0,0,0);
	DeviceContext->VSSetShader(0,0,0);
	DeviceContext->DSSetShader(0,0,0);
	DeviceContext->HSSetShader(0,0,0);

	//destroy shaders
	Shaders::VS_FullScreenQuad.Destroy();
	Shaders::VS_FullScreenQuadWOne.Destroy();
	Shaders::VS_PreHSPassThrough.Destroy();
	Shaders::VS_BV.Destroy();

	Shaders::PS_NormalBumpConverter.Destroy();
	Shaders::PS_Blur.Destroy();
	Shaders::PS_BV.Destroy();

	DestroyTrans_ToolBuffers();
	DestroyAABBBuffers();
	//destroy the textures

	Textures::RT_Base.Destroy();
	Textures::RT_Normal.Destroy();
	Textures::RT_DepthStencil.Destroy();
	Textures::RT_Depth.Destroy();
	Textures::RT_BackBufferTexture.Destroy();

	//destroy samplers
	Samplers::Nearest.Destroy();
	Samplers::Linear.Destroy();
	Samplers::BiLinear.Destroy();
	Samplers::TriLinear.Destroy();
	Samplers::Anisotropic.Destroy();

	Samplers::NearestClampUVW.Destroy();
	Samplers::LinearClampUVW.Destroy();
	Samplers::BiLinearClampUVW.Destroy();
	Samplers::TriLinearClampUVW.Destroy();
	Samplers::AnisotropicClampUVW.Destroy();

	//destroy rasterizers
	RasterizerStates::CullNone.Destroy();
	RasterizerStates::CullBack.Destroy();
	RasterizerStates::CullFront.Destroy();
	RasterizerStates::WireBack.Destroy();
	RasterizerStates::WireFront.Destroy();
	RasterizerStates::WireNoCull.Destroy();

	RasterizerStates::CullNone_NoDepthClip.Destroy();
	//destroy blendstates
	BlendStates::Add.Destroy();
	BlendStates::SrcAlpha.Destroy();
	BlendStates::No_Blend.Destroy();

	//destroy depthstates
	DepthStates::DepthTest.Destroy();
	DepthStates::StencilSet.Destroy();
	DepthStates::StencilTest.Destroy();
	DepthStates::StencilTestWDepthSet.Destroy();
	DepthStates::StencilSetWDepthTest.Destroy();
	DepthStates::StencilTestAndInc.Destroy();
	DepthStates::NoDepthTest.Destroy();
	DepthStates::NoDepthWrite.Destroy();
	DepthStates::DepthWriteNoDepthTest.Destroy();
	DepthStates::EqualDepthTest.Destroy();

	assert(Graphics::VertexShader::CachedShader.size()==0);
	assert(Graphics::GeometryShader::CachedShader.size()==0);
	assert(Graphics::HullShader::CachedShader.size()==0);
	assert(Graphics::DomainShader::CachedShader.size()==0);
	assert(Graphics::PixelShader::CachedShader.size()==0);

	if(DeviceContext)DeviceContext->OMSetRenderTargets(0, NULL, NULL); 
	if(SwapChain) SwapChain->SetFullscreenState( FALSE, NULL );// make sure to set to windowed state
	RELEASECOM(SwapChain);
	DeviceContext->ClearState();
	DeviceContext->Flush();
	RELEASECOM(DeviceContext);
	RELEASECOM(Device);
	OUTPUT_DEBUG_MSG("Finished Graphics::Internal::DeInit()");
}
void Graphics::Internal::OnResize(int x, int y){
	UnBindRenderTargets();
	Textures::RT_BackBufferTexture.Destroy();// make sure to free the resources 
	SwapChain->ResizeBuffers(1, x, y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	CreateBuffers(x, y);
	Textures::RT_Base.Resize(			x, y, 1, 1, 1);
	Textures::RT_Normal.Resize(			x, y, 1, 1, 1);
	Textures::RT_DepthStencil.Resize(	x, y, 1, 1, 1);
	Textures::RT_Depth.Resize(			x, y, 1, 1, 1);
	// let everyone know who registered that the screen has changed
	for(auto i(Internal::OnResizeCallBacks.begin()); i !=Internal::OnResizeCallBacks.end(); i++){
		i->func(x, y);
	}
}


void Graphics::CreateAABVBuffers(){
	vec3 points[] = {
		vec3(.5f, .5f, .5f),
		vec3(.5f,-.5f,.5f),
		vec3(.5f, -.5f, -.5f),
		vec3(.5f, .5f, -.5f),

		vec3(-.5f, .5f, .5f),
		vec3(-.5f,-.5f, .5f),
		vec3(-.5f, -.5f, -.5f),
		vec3(-.5f, .5f, -.5f),
		vec3(-.5f, .5f, -.5f)
	};
	uint16_t inds[] = {0,1,2,3,0,4,5,6, 7,4, 0, 5, 1, 6, 2, 7, 3,4};
	Internal_Components::VS_BV_VB.Create(sizeof(points)/sizeof(vec3), sizeof(vec3), BufferType::VERTEX_BUFFER, DEFAULT, CPU_NONE, points);
	Internal_Components::VS_BV_IB.Create(sizeof(inds)/sizeof(uint16_t), sizeof(uint16_t), BufferType::INDEX_BUFFER, DEFAULT, CPU_NONE, inds);
	Internal_Components::VS_BV_Cbuffer0.Create(1, sizeof(mat4) + sizeof(vec4), CONSTANT_BUFFER);
}
void Graphics::DestroyAABBBuffers(){
	Internal_Components::VS_BV_IB.Destroy();
	Internal_Components::VS_BV_VB.Destroy();
	Internal_Components::VS_BV_Cbuffer0.Destroy();
}

/*
world is the matrix of the object the AABV is holding. 
View and Proj are the cameras
center is the center point of the BV in ITS space
size_of_each_axis hold the TOTAL SIZE of each axis
*/
void Graphics::Draw_AABV(const mat4& view, const mat4& proj, const mat4& world, const vec3& center, const vec3& size_of_each_axis){
	Graphics::SetTopology(PRIM_LINE_STRIP);


	mat4 bvscale, bvtrans;
	bvscale.setupScale(size_of_each_axis);
	bvtrans.setupTranslation(center);

	Graphics::DepthStates::DepthTest.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::BlendStates::No_Blend.Bind();

	Shaders::VS_BV.Bind();
	Shaders::PS_BV.Bind();

	struct tempstruct{
		mat4 vp;
		vec4 color;
	};
	tempstruct t;
	t.vp = bvscale*bvtrans*world*view*proj;// we have to move the BV that was pregenerated into the correct position and scale it 
	t.vp.Transpose();
	t.color = vec4(1.0f, 0, 0, 1);//red
	Internal_Components::VS_BV_Cbuffer0.Update(&t);
	Internal_Components::VS_BV.SetConstantBuffer(Internal_Components::VS_BV_Cbuffer0);

	Internal_Components::VS_BV_IB.BindAsIndexBuffer();
	Internal_Components::VS_BV_VB.BindAsVertexBuffer();
	
	DrawIndexed(0, Internal_Components::VS_BV_IB.Size);
	
}

void Graphics::CreateTrans_ToolBuffers(){
	std::vector<vec3> points;
	std::vector<uint16_t> indices;
	points.push_back(vec3(1.0f, 0.0f, 0.0f));
	const float split=16.0f;
	//note: all of these shapes will have to be scaled to correctly in the draw function, but its not something the user will do. that will be done in the draw function below
	// first create the cone pointer
	for(float i=0.0f; i<2*Pi; i+=Pi/split){
		vec3 p;
		p.x=0.0f;
		p.y=sinf(i);
		p.z=cosf(i);
		points.push_back(p);
	}
	uint16_t index=1;
	for(float i=0.0f; i<2*Pi; i+=Pi/(split/2.0f)){
		indices.push_back(0);
		indices.push_back(index++);
		indices.push_back(index+1);
	}
	// now create the rod to connect to it 
	// a long triangle looks the same as a rod
	points.push_back(vec3(0.0f, 1.0f, 0.0f));//0
	points.push_back(vec3(1.0f, 0.0f, 0.0f));//1
	points.push_back(vec3(0.0f, 0.0f, 1.0f));//2

	points.push_back(vec3(0.0f, -1.0f, 0.0f));//3
	points.push_back(vec3(-1.0f, 0.0f, 0.0f));//4
	points.push_back(vec3(0.0f, 0.0f, -1.0f));//5
	Internal_Components::Trans_Ind_Cone_Start =0;
	Internal_Components::Trans_Ind_Cone_Start_Count = index;
	Internal_Components::Trans_Ind_Rod_Start =index-1;

	index=indices.size()-1;// get the index
	indices.push_back(index+0);
	indices.push_back(index+3);
	indices.push_back(index+1);

	indices.push_back(index+1);
	indices.push_back(index+4);
	indices.push_back(index+3);

	indices.push_back(index+1);
	indices.push_back(index+4);
	indices.push_back(index+2);

	indices.push_back(index+2);
	indices.push_back(index+5);
	indices.push_back(index+4);

	indices.push_back(index+2);
	indices.push_back(index+5);
	indices.push_back(index+0);

	indices.push_back(index+0);
	indices.push_back(index+5);
	indices.push_back(index+3);
	Internal_Components::Trans_Ind_Rod_Start_Count = index - Internal_Components::Trans_Ind_Cone_Start_Count;

	Internal_Components::VS_Trans_VB.Create(points.size(), sizeof(vec3), BufferType::VERTEX_BUFFER, DEFAULT, CPU_NONE, &points[0]);
	Internal_Components::VS_Trans_IB.Create(indices.size(), sizeof(uint16_t), BufferType::INDEX_BUFFER, DEFAULT, CPU_NONE, &indices[0]);
}
void Graphics::DestroyTrans_ToolBuffers(){
	Internal_Components::VS_Trans_VB.Destroy();
	Internal_Components::VS_Trans_IB.Destroy();
}
void Graphics::Draw_Trans_Tool(const mat4& view, const mat4& proj, const mat4& world){
	
	Graphics::SetTopology(PRIM_TRIANGLELIST);
	float avgaxis = (world._11 + world._22 + world._33)/3.0f;
	mat4 bvscale;
	bvscale.setupScale(avgaxis);

	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::RasterizerStates::CullBack.Bind();
	Graphics::BlendStates::No_Blend.Bind();

	Shaders::VS_BV.Bind();
	Shaders::PS_BV.Bind();

	struct tempstruct{
		mat4 vp;
		vec4 color;
	};
	tempstruct t;
	t.vp = bvscale*world*view*proj;// we have to move the BV that was pregenerated into the correct position and scale it 
	t.vp.Transpose();
	t.color = vec4(1.0f, 0, 0, 1);//red
	Internal_Components::VS_BV_Cbuffer0.Update(&t);
	Internal_Components::VS_BV.SetConstantBuffer(Internal_Components::VS_BV_Cbuffer0);

	Internal_Components::VS_Trans_VB.BindAsVertexBuffer();
	Internal_Components::VS_Trans_IB.BindAsVertexBuffer();
	
	DrawIndexed(Internal_Components::Trans_Ind_Cone_Start, Internal_Components::Trans_Ind_Cone_Start_Count);
	
}
