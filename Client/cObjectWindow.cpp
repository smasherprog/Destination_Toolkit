#include "stdafx.h"
#include "cObjectWindow.h"
#include "commdlg.h"
#include "../MY_UI\DropDown.h"
#include "../MY_UI\Tab_Control.h"
#include "../MY_UI\Slider.h"
#include "../MY_UI\ColorPickerWindow.h"
#include "../MY_UI\Scrollable.h"
#include "../MY_UI\cWidgetEvents.h"
#include "Terrain.h"
#include "../Graphics_Lib/MeshTypes.h"
#include "cDemoControl.h"

cObjectWindow* cObjectWindow::self=0;

cObjectWindow::cObjectWindow(cWidgetBase* parent, cDemoControl* demo) : MY_UI::Controls::Window(parent), DemoControl(*demo) {
	assert(self ==0);
	SetTitle("Object Options");

	MY_UI::Controls::Tab_Control* TabControl = new MY_UI::Controls::Tab_Control( this );	
	MY_UI::Controls::Simple_Window* wnd = TabControl->Insert( "Mesh Options" );
	TabControl->SetPos(2, 2);
	TabControl->Align_Within_Parent(FILL);

	SelectType = new MY_UI::Controls::DropDown( wnd );
	SelectType->SetPos(2, 2);
	SelectType->Insert( "Mesh", MESH);
	SelectType->Insert( "Point Light", POINTLIGHT );
	SelectType->Insert( "Woody3d Tree", WOODYTREEMESH);
	SelectType->SetLength(105);
	
	MY_UI::Controls::Button* img = new MY_UI::Controls::Button( wnd );
	img->Align_To_Widget(SelectType, RIGHT | IN_LINE, 10);
	img->SetText("Load Object");
	img->OnLeftClicked.Add(std::bind(&cObjectWindow::LoadObject, this));
	
	wnd = TabControl->Insert( "Terrain Options" );
	/////////combobox
	MY_UI::Controls::DropDown* combo = new MY_UI::Controls::DropDown( wnd );
	combo->SetPos(2, 22);
	combo->SetLength( 120 );
	combo->Insert( "Editing_Off", 5 );
	combo->Insert( "Raise", 0 );
	combo->Insert( "Lower", 1);
	combo->Insert( "Roughen", 2 );
	combo->Insert( "Smooth", 3 );
	combo->Insert( "Texture", 4);
	
	combo->SetLink(&DemoControl.Terr.EditorFunction);
	
	MY_UI::Controls::HorizontalSlider_w_Label<float>* hori = new MY_UI::Controls::HorizontalSlider_w_Label<float>(wnd);
	hori->SetLabel("Inner Radius");
	hori->Widget->SetLink(&DemoControl.Terr.EditorSize);
	hori->Widget->SetRange( 5, 100 );
	hori->Align_To_Widget(combo, BOTTOM | IN_LINE, 10);

	MY_UI::Controls::HorizontalSlider_w_Label<float>* hori1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(wnd);
	hori1->SetLabel("Outer Radius");
	hori1->Widget->SetLink(&DemoControl.Terr.EditorSize);//<-------fix link later
	hori1->Widget->SetRange( 5, 100 );	
	hori1->Align_To_Widget(hori, BOTTOM | IN_LINE, 5);

	
	MY_UI::Controls::ColorPicker_w_Label* picker = new MY_UI::Controls::ColorPicker_w_Label(wnd);
	picker->SetLabel("Editor Color ", LEFT | IN_LINE, 5);
	picker->Align_To_Widget(hori1, BOTTOM | IN_LINE, 10);
	picker->Widget->SetLink(&DemoControl.Terr.EditorColor.x, &DemoControl.Terr.EditorColor.y, &DemoControl.Terr.EditorColor.z, &DemoControl.Terr.EditorColor.w);

	wnd = TabControl->Insert( "Textures" );
	MY_UI::Controls::Scrollable* pCtrl = new MY_UI::Controls::Scrollable( wnd );
	pCtrl->Align_Within_Parent(FILL);
	
	int col=-1;
	int row=0;
	for(int i(0); i< sizeof(DiffuseTextures)/sizeof(std::string); i++){
		if(col==10) {
			col = -1;
			row++;
		}
		col++;
		MY_UI::Controls::Button* img = new MY_UI::Controls::Button( pCtrl );
		img->ClearImage(3);
		img->ClearImage(2);
		img->ClearImage(1);
		// leave only the un pressed image
		
		img->Internal_Image.Textures[0].Texture.Tex= DemoControl.Terr.Diffuse.SrvArray[i];// manual set of the pressed image
		img->Internal_Image.Textures[0].OwnsTexture = false;
		img->Internal_Image.Textures[0].Texture.height = img->Internal_Image.Textures[0].Texture.width = 1024;
		img->SetId(i);
		img->SetBorderDrawn(false);
		img->OnLeftClicked.Add(std::bind(&cObjectWindow::TextureChanged, this, std::placeholders::_1), true);
		img->SetUVs(0.0f, 0.0f, 1.0f, 1.0f);
		img->SetSize(100, 100);
		img->SetPos(col*100 + 10, row*100 + 10);
	}
	
	SetSize(400, 300);
	SetPos(100, 100);
}
void cObjectWindow::TextureChanged(MY_UI::Controls::cWidgetBase* pControl ){

	DemoControl.Terr.SelectedTex = static_cast<unsigned char>(pControl->GetId());
	OUTPUT_DEBUG_MSG("changing tex to "<<DemoControl.Terr.SelectedTex);
}
void cObjectWindow::LoadObject( ){
	MY_UI::Controls::Button* b = SelectType->GetSelected();
	if(b->GetId() >= NULL_TYPE) return;
	if(b->GetId()  == POINTLIGHT){
		cBaseMesh* temp = DemoControl.LoadObject("", Camera.Position, "Unamed Point Light", b->GetId());
		DemoControl.SetTarget(temp);
		return;
	}
	/*
	char szFile[MAX_PATH];
	szFile[0] = 0;
	OPENFILENAMEA ofn;
	ofn.lpstrFile = szFile;
    memset(&ofn, 0, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	std::string startingname;
	if(b->GetId() == WOODYTREEMESH){
		ofn.lpstrFilter = "Woody Trees\0*.wdmesh;\0";
		OUTPUT_DEBUG_MSG("Atempting to load the Woody tree mesh file "<<szFile);
		startingname = "Woodytree Mesh Unamed";
		GetOpenFileNameA( &ofn );
	} else {// must be a mesh type
		std::string ext=  DemoControl.ObjectContainer.ValidModelFiles + ";*.dsm;";
		ext.push_back('h');
		ext.back() = '\0';
		ofn.lpstrFilter = ext.c_str();	
		startingname = " Mesh Unamed";
		GetOpenFileNameA( &ofn );
	}
	
	std::string fname = szFile;// makes it easier to work with
	if(fname.size() < 3){
		SetCurrentDirectoryA(AbsolutePath.c_str());
		return;// done
	}
	cBaseMesh* temp = DemoControl.ObjectContainer.LoadObject(fname, Camera.Position, startingname, b->GetId() );
	DemoControl.SetTarget(temp);
	SetCurrentDirectoryA(AbsolutePath.c_str());
	*/
}

/*
	TwRemoveVar(MeshLoader, "MeshfName");
	TwAddVarCB(MeshLoader,	"MeshfName",	TW_TYPE_STDSTRING,	cBaseMesh::SetName,			cBaseMesh::GetName,			ob,	"label='Name'");
	//ROTATION
	TwRemoveVar(MeshLoader, "Rotation");
	TwAddVarCB(MeshLoader,	"Rotation",		TW_TYPE_QUAT4F,		cBaseMesh::SetRotation,		cBaseMesh::GetRotation,		ob,	"Group='rot_info'");
	TwRemoveVar(MeshLoader, "rot.x");
	TwAddVarCB(MeshLoader,	"rot.x",		TW_TYPE_FLOAT,		cBaseMesh::SetRotationX,	cBaseMesh::GetRotationX,	ob,	"Group='rot_info'");
	TwRemoveVar(MeshLoader, "rot.y");
	TwAddVarCB(MeshLoader,	"rot.y",		TW_TYPE_FLOAT,		cBaseMesh::SetRotationY,	cBaseMesh::GetRotationY,	ob,	"Group='rot_info'");
	TwRemoveVar(MeshLoader, "rot.z");
	TwAddVarCB(MeshLoader,	"rot.z",		TW_TYPE_FLOAT,		cBaseMesh::SetRotationZ,	cBaseMesh::GetRotationZ,	ob,	"Group='rot_info'");
	//POSITON
	TwRemoveVar(MeshLoader, "pos.x");
	TwAddVarCB(MeshLoader,	"pos.x",		TW_TYPE_FLOAT,		cBaseMesh::SetPositionX,	cBaseMesh::GetPositionX,	ob,	"Group='pos_info'");
	TwRemoveVar(MeshLoader, "pos.y");
	TwAddVarCB(MeshLoader,	"pos.y",		TW_TYPE_FLOAT,		cBaseMesh::SetPositionY,	cBaseMesh::GetPositionY,	ob,	"Group='pos_info'");
	TwRemoveVar(MeshLoader, "pos.z");
	TwAddVarCB(MeshLoader,	"pos.z",		TW_TYPE_FLOAT,		cBaseMesh::SetPositionZ,	cBaseMesh::GetPositionZ,	ob,	"Group='pos_info'");
	//SCALING
	TwRemoveVar(MeshLoader, "scale.x");
	TwAddVarCB(MeshLoader,	"scale.x",		TW_TYPE_FLOAT,		cBaseMesh::SetScalingX,		cBaseMesh::GetScalingX,		ob,	"Group='scale_info' Min=.1 Max=100 Step=.1");
	TwRemoveVar(MeshLoader, "scale.y");
	TwAddVarCB(MeshLoader,	"scale.y",		TW_TYPE_FLOAT,		cBaseMesh::SetScalingY,		cBaseMesh::GetScalingY,		ob,	"Group='scale_info' Min=.1 Max=100 Step=.1");
	TwRemoveVar(MeshLoader, "scale.z");
	TwAddVarCB(MeshLoader,	"scale.z",		TW_TYPE_FLOAT,		cBaseMesh::SetScalingZ,		cBaseMesh::GetScalingZ,		ob,	"Group='scale_info' Min=.1 Max=100 Step=.1");
	//ANIMATION RELATED STUFF
	TwRemoveVar(MeshLoader, "Animations");
	TwRemoveVar(MeshLoader, "Animation_Speed");
	if(ob->HasBones()){
		
		cMesh* mesh = static_cast<cMesh*>(ob);// valid cast
		SceneAnimator* an =&mesh->mAnimator;

		std::vector<TwEnumVal> anims(an->Animations.size());
		for(size_t i(0); i< an->Animations.size(); i++){
			anims[i].Value = (int)i;
			anims[i].Label = an->Animations[i].Name.c_str();
		}
		
		TwType animtstypes = TwDefineEnum("Animslist", &anims[0], static_cast<unsigned int>(anims.size()));  
		TwAddVarRW(MeshLoader, "Animations",				animtstypes,	  &an->CurrentAnimIndex,				"Group='Anims'");	
		if(!an->Animations.empty()){
			TwAddVarCB(MeshLoader,	"Animation_Speed",		TW_TYPE_FLOAT,		SetAnimationSpeed,	GetAnimationSpeed,	an,	"Group='Anims' Min=10 Max=400 Step=1");
		}
	} 
	//vec4 Diffuse, SpecularColor;
	TwRemoveVar(MeshLoader, "Light_Color");
	TwRemoveVar(MeshLoader, "Light_Spec");
	TwRemoveVar(MeshLoader, "Light_Atten1");
	if(ob->GetType() == POINTLIGHT){
		cPointLight* light = static_cast<cPointLight*>(ob);// valid cast	
		TwAddVarRW(MeshLoader, "Light_Color", TW_TYPE_COLOR4F, &light->Diffuse , " Group='Lights' Label=Color ");
		TwAddVarRW(MeshLoader, "Light_Spec", TW_TYPE_COLOR4F, &light->SpecularColor , " Group='Lights' Label=Spec_Color ");
		TwAddVarRW(MeshLoader, "Light_Atten1", pointType,		&light->Attenuation, " Group='Lights'");
	}
	*/

cObjectWindow::~cObjectWindow(){
	self=0;
}