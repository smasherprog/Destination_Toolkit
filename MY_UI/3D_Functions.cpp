#include "stdafx.h"
#include "Test_Helper.h"
#include "MY_UI_Test.h"
#include "Window.h"
#include "DropDown.h"
#include "Text.h"
#include "TextBox.h"
#include "ColorPickerWindow.h"
#include "Objects_3d.h"
#include "ApplicationLayer.h"
#include "UI_Camera_Example.h"
#include "../Utilities/MyMath.h"
#include "CheckBox.h"

#if defined(DEBUG) | defined (_DEBUG)
#include <dxerr.h>
#pragma comment(lib, "dxerr") // needed for dxtrace
#ifndef HR
#define HR(x){																\
	HRESULT hr = (x);														\
	if(FAILED(hr)){															\
	std::cout<<DXGetErrorStringA(hr)<<std::endl<<DXGetErrorDescriptionA(hr)<<std::endl;   \
	assert(0);															\
	}																		\
		}
#endif
#endif
#ifndef HR
#define HR(x) (x)
#endif

bool Drawing3dFunctions=false;

mat4 mWorld;
mat4 mRotation;
mat4 mTranslation;
mat4 mScaling;
quat Rot;



MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::CheckBox>* trans_checkbox=0;
UI_Camera_Example* Cam=0;
MY_UI::Controls::Translator*  ObjectTranslation=0; 
MY_UI::Controls::Rotator*  ObjectRotator=0; 
MY_UI::Controls::Scaler*  ObjectScaler=0; 
struct Vertex{
	vec4 pos;
	vec4   color;
};
const int mNumVertices = 8;
const int mNumFaces    = 12; // 2 per quad
Vertex Verticies[mNumVertices];

////////////////////////////////////////////////////GUI STUFF
void Clearall(){
	delete ObjectTranslation;
	ObjectTranslation=0;
	delete ObjectScaler;
	ObjectScaler =0;
	delete ObjectRotator;
	ObjectRotator=0;
}
void StopDrawing(){
	Drawing3dFunctions=false;
	MY_UI::Internal::RootWidget->GetApplicationLayer()->SetCamera(0);// this will delete the camera class and stop it from catching input
	Cam=0;
	// the Dummy will delete the child when it is destroyed You could use MY_UI::Safe_Delete here   That is the purpose of it. If you are unsure whether a control has been deleted, but you want to make sure it is, call safe_delete on the object
	ObjectTranslation=0;
	ObjectRotator=0;
	ObjectScaler=0;
	trans_checkbox=0;
	//Optionally
	//MY_UI::Safe_Delete(ObjectTranslation);
	//MY_UI::Safe_Delete(ObjectRotator);
}

void CreateRotator(){
	Clearall();
	trans_checkbox->Widget->SetLink(0);// make sure to clear this
	ObjectRotator = new MY_UI::Controls::Rotator(Dummy);
	// i can pass the size of the cube in as 1.0f because that is the untransformed dimensions of the cube!
	ObjectRotator->Set_Links(&mScaling, &mRotation, &Rot, &mTranslation, &Cam->mProj, &Cam->mView, 1.0f, 1.0f, 1.0f);
}
void CreateTranslator(){
	Clearall();
	ObjectTranslation = new MY_UI::Controls::Translator(Dummy);
	// i can pass the size of the cube in as 1.0f because that is the untransformed dimensions of the cube!
	ObjectTranslation->Set_Links(&mScaling, &mRotation, &mTranslation,&Cam->mProj, &Cam->mView, 1.0f, 1.0f, 1.0f);
	trans_checkbox->Widget->SetLink(&ObjectTranslation->WorldSpaceEditing);
}
void CreateScaler(){
	Clearall();
	trans_checkbox->Widget->SetLink(0);// make sure to clear this
	ObjectScaler = new MY_UI::Controls::Scaler(Dummy);
	// i can pass the size of the cube in as 1.0f because that is the untransformed dimensions of the cube!
	ObjectScaler->Set_Links(&mScaling, &mRotation, &mTranslation, &Cam->mProj, &Cam->mView, 1.0f, 1.0f, 1.0f);
}

void Functions3D(){	
	Rot.identity();
	mWorld.identity();
	mRotation.identity();
	mTranslation.identity();
	mScaling.identity();

	delete Dummy;
	Dummy = new MY_UI::Controls::cWidget_Dummy(root);
	Dummy->SetControlBounds(root->GetControlBounds());
	Dummy->SetClientBounds(root->GetClientBounds());
	InitBox();
	Cam=new UI_Camera_Example();
	MY_UI::Internal::RootWidget->GetApplicationLayer()->SetCamera(Cam);// applicationlayer will clean up the resources

	MY_UI::Controls::Simple_Window* window = new MY_UI::Controls::Simple_Window(Dummy);
	window->SetPos((rand()% 900), (rand()% 500));
	window->SetSize(325, 200);
	window->OnDestroyed.Add(StopDrawing);// so the cube doesnt drawn

	MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>* scaling = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>(window);
	scaling->SetLabel("Scaling", TOP | CENTER, 5);
	scaling->Widget->Set_Box_Length(65);
	scaling->Widget->Set_Box_Alignment(BOTTOM);
	scaling->Widget->Set_Box_Increment(.01f);
	scaling->Widget->Set_Box_Links(&mScaling._11, &mScaling._22, &mScaling._33);
	scaling->SetPos(5, 5);// important to setpos after the alignment is set

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(window);
	pbutton->SetText("Scaler", CENTER);
	pbutton->OnLeftClicked.Add(CreateScaler);
	pbutton->Align_To_Widget(scaling, BOTTOM | CENTER, 5);

	MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>* position = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>(window);
	position->SetLabel("Position", TOP | CENTER, 5);
	position->Widget->Set_Box_Length(65);
	position->Widget->Set_Box_Alignment(BOTTOM);
	position->Widget->Set_Box_Increment(.01f);
	position->Widget->Set_Box_Range(-200.0f, 200.0f);
	position->Widget->Set_Box_Links(&mTranslation._41, &mTranslation._42, &mTranslation._43);
	position->Align_To_Widget(scaling, RIGHT | IN_LINE, 10);
	
	pbutton = new MY_UI::Controls::Button(window);
	pbutton->SetText("Translator", CENTER);
	pbutton->OnLeftClicked.Add(CreateTranslator);
	pbutton->Align_To_Widget(position, BOTTOM | CENTER, 5);

	trans_checkbox = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::CheckBox>(window);
	trans_checkbox->SetLabel("World Space", TOP | CENTER, 5);
	trans_checkbox->Align_To_Widget(pbutton, BOTTOM | IN_LINE, 5);
	
	scaling = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>(window);
	scaling->SetLabel("Rotation", TOP | CENTER, 5);
	scaling->Widget->Set_Box_Length(65);
	scaling->Widget->Set_Box_Alignment(BOTTOM);
	scaling->Widget->Set_Box_Increment(.01f);
	scaling->Widget->Set_Box_Links(&Rot.x, &Rot.y, &Rot.z);
	scaling->Widget->Set_Box_Range(-999.0f, 999.0f);
	scaling->Align_To_Widget(position, RIGHT | IN_LINE, 10);

	pbutton = new MY_UI::Controls::Button(window);
	pbutton->SetText("Rotator", CENTER);
	pbutton->OnLeftClicked.Add(CreateRotator);
	pbutton->Align_To_Widget(scaling, BOTTOM | CENTER, 5);

	MY_UI::Controls::Text* text = new 	MY_UI::Controls::Text(window);
	text->SetText("Change Colors");
	text->Align_To_Widget(scaling, RIGHT | IN_LINE, 15);
	text->SetSizeToContents();

	MY_UI::Controls::ColorPicker* picker = new MY_UI::Controls::ColorPicker(window);
	picker->Align_To_Widget(text, BOTTOM | IN_LINE, 10);
	picker->SetLink(&Verticies[0].color.x, &Verticies[0].color.y, &Verticies[0].color.z, &Verticies[0].color.w);
	picker->OnColorSelected.Add(InitBox);// rebuild the box

	MY_UI::Controls::ColorPicker* picker1 = new MY_UI::Controls::ColorPicker(window);
	picker1->Align_To_Widget(picker, RIGHT | IN_LINE, 5);
	picker1->SetLink(&Verticies[1].color.x, &Verticies[1].color.y, &Verticies[1].color.z, &Verticies[1].color.w);
	picker1->OnColorSelected.Add(InitBox);// rebuild the box

	picker = new MY_UI::Controls::ColorPicker(window);
	picker->Align_To_Widget(picker1, BOTTOM | IN_LINE, 5);
	picker->SetLink(&Verticies[2].color.x, &Verticies[2].color.y, &Verticies[2].color.z, &Verticies[2].color.w);
	picker->OnColorSelected.Add(InitBox);// rebuild the box

	picker1 = new MY_UI::Controls::ColorPicker(window);
	picker1->Align_To_Widget(picker, LEFT | IN_LINE, 5);
	picker1->SetLink(&Verticies[3].color.x, &Verticies[3].color.y, &Verticies[3].color.z, &Verticies[3].color.w);
	picker1->OnColorSelected.Add(InitBox);// rebuild the box

	Drawing3dFunctions= true;
}

////////////////////////////////////////////////////RENDERING AND DIRECT X SPECIFIC STUFF BELOW!!

inline HRESULT CompileShaderFromMemory(const std::string& strinmem, const std::string& functionname,const std::string& shadermodel, ID3DBlob** ppBlobOut ){
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;

    ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromMemory(strinmem.c_str(), strinmem.size(), NULL, NULL, NULL, functionname.c_str(), shadermodel.c_str(),  dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) ){
        if( pErrorBlob != NULL )
            OUTPUT_DEBUG_MSG( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

ID3D11VertexShader*     VertexShader = NULL;
ID3D11PixelShader*      PixelShader = NULL;

ID3D11InputLayout *     g_VertexLayout = NULL;

ID3D11Buffer *          g_VertexBuffer = NULL;
ID3D11Buffer *          g_IndexBuffer = NULL;
ID3D11Buffer*			mfxWVPVar=NULL;
ID3D11Texture2D*		mDepthStencilBuffer=0;
ID3D11DepthStencilView* mDepthStencilView=0;


const float PI       = 3.14159265358979323f;

const vec4 WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const vec4 RED(1.0f, 0.0f, 0.0f, 1.0f);
const vec4 GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const vec4 BLUE(0.0f, 0.0f, 1.0f, 1.0f);
const vec4 YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
const vec4 CYAN(0.0f, 1.0f, 1.0f, 1.0f);
const vec4 MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);

// Initialize the 3D objects & effects
bool InitScene(){

    char shaderFX[] = 
		"cbuffer cbPerObject{"
			"matrix gWVP;" 
		"};"
		"struct PS_INPUT{"
			"float4 PosH : SV_POSITION;"
			"float4 Color : COLOR;"
		"};"
		"PS_INPUT VS(float4 iPosL  : POSITION, float4 iColor : COLOR){"
	// Transform to homogeneous clip space.
			"PS_INPUT o;"
			"o.PosH = mul(iPosL, gWVP);"
	// Just pass vertex color into the pixel shader.
			"o.Color = iColor;"
			"return o;"
		"}"

		"float4 PS(float4 posH  : SV_POSITION, float4 color : COLOR) : SV_Target{"
			"return color;"
		"}";
	ID3DBlob* blob=0;
	HR(CompileShaderFromMemory(shaderFX, "PS", "ps_4_0",  &blob)); 	
	HR(g_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, &PixelShader));
	RELEASECOM(blob);
	HR(CompileShaderFromMemory(shaderFX, "VS", "vs_4_0",  &blob)); 	
	HR(g_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, &VertexShader));
	

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] ={
        { "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
        { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    HR(g_Device->CreateInputLayout(layout, sizeof(layout)/sizeof(layout[0]),  blob->GetBufferPointer(), blob->GetBufferSize(), &g_VertexLayout));
	RELEASECOM(blob);


	D3D11_BUFFER_DESC bd;
	memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(mat4);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR(g_Device->CreateBuffer( &bd, NULL, &mfxWVPVar ));

	MY_UI::Utilities::Point size = MY_UI::Internal::RootWidget->GetSize();

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = size.x;
	depthStencilDesc.Height    = size.y;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
	depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;
	  
	g_Device->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer);
	g_Device->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView);

	// Bind the render target view and depth/stencil view to the pipeline.
	ID3D11RenderTargetView* rtv [1];
	ID3D11DepthStencilView* dsv[1];
	g_DeviceContext->OMGetRenderTargets(1, rtv, dsv);
	g_DeviceContext->OMSetRenderTargets(1, rtv, mDepthStencilView);
	
		// Create vertex buffer

	Verticies[0].pos = vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	Verticies[0].color = WHITE;
	Verticies[1].pos = vec4(-1.0f, +1.0f, -1.0f, 1.0f);
	Verticies[1].color = BLACK;
	Verticies[2].pos = vec4(+1.0f, +1.0f, -1.0f, 1.0f);
	Verticies[2].color = RED;
	Verticies[3].pos = vec4(+1.0f, -1.0f, -1.0f, 1.0f);
	Verticies[3].color = GREEN;
	Verticies[4].pos = vec4(-1.0f, -1.0f, +1.0f, 1.0f);
	Verticies[4].color = BLUE;
	Verticies[5].pos = vec4(-1.0f, +1.0f, +1.0f, 1.0f);
	Verticies[5].color = YELLOW;
	Verticies[6].pos = vec4(+1.0f, +1.0f, +1.0f, 1.0f);
	Verticies[6].color = CYAN;
	Verticies[7].pos = vec4(+1.0f, -1.0f, +1.0f, 1.0f);
	Verticies[7].color = MAGENTA;

	InitBox();

	return true;
}

// Clean up D3D objects
void Cleanup(){

	RELEASECOM(VertexShader);
	RELEASECOM(PixelShader);
	RELEASECOM(g_IndexBuffer);
	RELEASECOM(mfxWVPVar);
	
	RELEASECOM(g_VertexBuffer);
	RELEASECOM(g_VertexLayout);
	RELEASECOM(mDepthStencilBuffer);
	RELEASECOM(mDepthStencilView);

}


// Render scene
void Render3D(){
	if(!Drawing3dFunctions) return;

	Cam->Update();// rebuild the view matrix each frame
	g_DeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	mRotation.setupfromquat(Rot);
	mWorld = mScaling * mRotation * mTranslation;

	mat4 mWVP = mWorld*Cam->mView*Cam->mProj;
	mWVP.Transpose();// must transpose before setting
	g_DeviceContext->UpdateSubresource( mfxWVPVar, 0, NULL, &mWVP, 0, 0 );

	g_DeviceContext->VSSetShader(VertexShader, 0, 0);
	g_DeviceContext->PSSetShader(PixelShader, 0, 0);
	g_DeviceContext->VSSetConstantBuffers(0, 1, &mfxWVPVar); 
	g_DeviceContext->IASetInputLayout(g_VertexLayout);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_DeviceContext->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
	g_DeviceContext->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_DeviceContext->RSSetState(0);
 	g_DeviceContext->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

    g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_DeviceContext->DrawIndexed(mNumFaces*3, 0, 0);
  
}


void InitBox(){
	RELEASECOM(g_VertexBuffer);

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &Verticies[0];
    HR(g_Device->CreateBuffer(&vbd, &vinitData, &g_VertexBuffer));


	// Create the index buffer

	unsigned int indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};
	RELEASECOM(g_IndexBuffer);

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned int) * mNumFaces*3;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    HR(g_Device->CreateBuffer(&ibd, &iinitData, &g_IndexBuffer));
}

