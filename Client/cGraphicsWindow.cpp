#include "stdafx.h"
#include "cGraphicsWindow.h"
#include "cDemoControl.h"
#include "../Graphics_Lib/cCamera.h"
#include "Global_GameData.h"
#include "../MY_UI\Tab_Control.h"
#include "../MY_UI\CheckBox.h"
#include "../MY_UI\DropDown.h"
#include "../MY_UI\Scrollable.h"
#include "../MY_UI\Slider.h"

cGraphicsWindow* cGraphicsWindow::self=0;

cGraphicsWindow::cGraphicsWindow(MY_UI::Controls::cWidgetBase* parent, cDemoControl& DemoControl): Window(parent){
	self=this;
	SetTitle("Graphics Options");
	SetSize(550, 600);
	
	MY_UI::Controls::Tab_Control* m_TabControl = new MY_UI::Controls::Tab_Control( this );
	m_TabControl->Align_Within_Parent( FILL );

	MY_UI::Controls::Simple_Window* pButton = m_TabControl->Insert( "Drawing Options" );

	MY_UI::Controls::Scrollable* pCtrl = new MY_UI::Controls::Scrollable( pButton );
	pCtrl->Align_Within_Parent(FILL);
	

	MY_UI::Controls::CheckBox_w_Label* labeled = new MY_UI::Controls::CheckBox_w_Label( pCtrl );
	labeled->SetLabel( "Use FXAA(Blurs Edges)" , LEFT | IN_LINE, 5);
	labeled->Widget->SetLink(&DemoControl.FXAA.Blending);
	labeled->SetPos( 10, 10 );

	MY_UI::Controls::HorizontalSlider_w_Label<float>* hor = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
	hor->SetLabel("Level Of Detail 1 Draw Distance", TOP|IN_LINE, 0);
	hor->Widget->SetRange(100, 1000 );
	hor->Widget->SetLength(200);
	hor->Widget->SetLink(&DemoControl.ObjectContainer.Lod1Dist);
	hor->Align_To_Widget(labeled, BOTTOM |IN_LINE, 0);
	
	MY_UI::Controls::HorizontalSlider_w_Label<float>* hor1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
	hor1->SetLabel("Level Of Detail 2 Draw Distance", TOP| IN_LINE, 0);
	hor1->Widget->SetRange(1001, 5000);
	hor1->Widget->SetLength(200);
	hor1->Widget->SetLink(&DemoControl.ObjectContainer.Lod2Dist);
	hor1->Align_To_Widget(hor, BOTTOM |IN_LINE, 0);

	hor = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
	hor->SetLabel("Camera Near Plane", TOP| IN_LINE, 0);
	hor->Widget->SetRange(1, 50 );
	hor->Widget->SetLength(200);
	hor->Widget->SetLink(&Camera.NearPlane);
	hor->Align_To_Widget(hor1, BOTTOM |IN_LINE, 0);

	hor1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
	hor1->SetLabel("Camera Far Plane", TOP|IN_LINE, 0);
	hor1->Widget->SetRange(51, 10000);
	hor1->Widget->SetPrecision(5);
	hor1->Widget->SetLength(200);
	hor1->Widget->SetLink(&Camera.FarPlane);
	hor1->Align_To_Widget(hor, BOTTOM |IN_LINE, 0);
			
		MY_UI::Controls::DropDown* combo = new MY_UI::Controls::DropDown( pCtrl );
		combo->SetLength( 100 );
		combo->Insert( "No_Debug", -1 );
		combo->Insert( "Show_Normals", 4 );
		combo->Insert( "Show_Depth", 0 );
		combo->Insert( "Show_Diffuse", 1 );
		combo->Insert( "Show_Shadow", 5 );
		combo->Insert( "Show_Specular", 2 );
		combo->Insert( "Show_Stencil", 3 );
//		combo->SetLink(&DemoControl.DebugTech.ID);
		combo->Align_To_Widget(hor1, BOTTOM | IN_LINE, 0);
		

		MY_UI::Controls::Button* btn = new MY_UI::Controls::Button( pCtrl );
		btn->Align_To_Widget(combo, RIGHT | IN_LINE, 20);
		btn->SetText("Reload Shaders");
		btn->SetLength(btn->GetLength() + 20);
		btn->SetText("Reload Shaders");
		btn->OnLeftClicked.Add(std::bind(&cGraphicsWindow::ReloadShaders, this) );

		labeled = new MY_UI::Controls::CheckBox_w_Label( pCtrl );	
		labeled->SetLabel( "Show Lights" );
		labeled->Widget->SetLink(&DemoControl.ObjectContainer.ShowPointLights);
		labeled->Align_To_Widget(combo, BOTTOM | IN_LINE, 5);


		MY_UI::Controls::CheckBox_w_Label* labeled1 = new MY_UI::Controls::CheckBox_w_Label( pCtrl );	
		labeled1->SetLabel( "Show_Shadow_Splits" );
		labeled1->Align_To_Widget(labeled, BOTTOM | IN_LINE, 5);
		labeled1->Widget->SetLink(&DemoControl.GlobalLight.ShowSplits);

		hor1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
		hor1->SetLabel("Shadow Projection_Size", TOP|CENTER, 3);
		hor1->Widget->SetRange(500, 10000);
		hor1->Widget->SetPrecision(5);
		hor1->Widget->SetLength(200);
		hor1->Widget->SetLink(&DemoControl.GlobalLight.ProjSize);
		hor1->Align_To_Widget(labeled1, BOTTOM |IN_LINE, 5);

		hor = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
		hor->SetLabel("ShadowMap Distance", TOP| IN_LINE, 3);
		hor->Widget->SetRange( 0, 10000);
		hor->Widget->SetPrecision(5);
		hor->Widget->SetLength(200);
		hor->Widget->SetLink(&DemoControl.GlobalLight.ShadowMapDistance);
		hor->Align_To_Widget(hor1, BOTTOM |IN_LINE, 5);
			
		hor1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
		hor1->SetLabel("Lambda Split", TOP|CENTER, 3);
		hor1->Widget->SetRange( .3f, 1.0f);
		hor1->Widget->SetIncrement(.01f);
		hor1->Widget->SetLength(200);
		hor1->Widget->SetLink(&DemoControl.GlobalLight.Lambda);
		hor1->Align_To_Widget(hor, BOTTOM |IN_LINE, 5);

		combo = new MY_UI::Controls::DropDown( pCtrl );
		combo->SetLength( 100 );
		combo->Insert( "512x512", 512 );
		btn = combo->Insert( "1024x1024", 1024 );
		combo->Insert( "2048x2048", 2048 );
		combo->SetLink(&DemoControl.GlobalLight.ChangeShadowMap);
		combo->SetPos(275, 10);
		combo->SetSelected(btn);

		MY_UI::Controls::DropDown* combo1 = new MY_UI::Controls::DropDown( pCtrl );
		combo1->SetLength( 100 );
		combo1->Insert( "1x1", 0);
		combo1->Insert( "3x3", 3);
		btn = combo1->Insert( "5x5", 5);
		combo1->Insert( "7x7", 7 );
		combo1->Insert( "9x9", 9);
		combo1->Insert( "11x11", 11);
		combo1->Insert( "13x13", 13 );
		combo1->SetLink(&DemoControl.GlobalLight.BlurSamples);
		combo1->Align_To_Widget( combo, BOTTOM | IN_LINE, 5);	
		combo1->SetSelected(btn);

		combo = new MY_UI::Controls::DropDown( pCtrl );
		combo->SetLength( 100 );
		combo->Insert( "No_Weather", 0 );
		combo->Insert( "Heavy_Rain", 1 );
		combo->Insert( "Light_Rain", 2 );
		combo->Insert( "Light_Snow", 3 );
		combo->Insert( "Heavy_Snow", 4 );
		combo->SetLink(&DemoControl.Weather.BlendTo);
		combo->Align_To_Widget(	combo1, BOTTOM | IN_LINE, 5);	

		hor1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
		hor1->SetLabel("Cloud Cover", TOP|CENTER, 3);
		hor1->Widget->SetRange( 0.0f, 1.0f);
		hor1->Widget->SetIncrement(.01f);
		hor1->Widget->SetLength(200);
		hor1->Widget->SetLink(&DemoControl.Clouds.m_fCloudCover);
		hor1->Align_To_Widget(combo, BOTTOM |IN_LINE, 5);

		hor = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
		hor->SetLabel("Sun Speed", TOP| IN_LINE, 3);
		hor->Widget->SetRange( 0.0f, .5f);
	
		hor->Widget->SetLength(200);
		hor->Widget->SetLink(&SunSpeed);
		hor->Align_To_Widget(hor1, BOTTOM |IN_LINE, 5);

		hor1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
		hor1->SetLabel("Ocean HDR Exposure", TOP|CENTER, 3);
		hor1->Widget->SetRange( 0.0f, 1.0f);
		hor1->Widget->SetIncrement(.05f);
		hor1->Widget->SetLength(200);
		hor1->Widget->SetLink(&DemoControl.Ocean.hdrExposure);
		hor1->Align_To_Widget(hor, BOTTOM |IN_LINE, 5);

		hor = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
		hor->SetLabel("Ocean_Wind", TOP| IN_LINE, 3);
		hor->Widget->SetRange( 1.0f, 25.0f);
		hor->Widget->SetLength(200);
		hor->Widget->SetLink(&DemoControl.Ocean.Wind);
		hor->Align_To_Widget(hor1, BOTTOM |IN_LINE, 5);

		hor1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(pCtrl);
		hor1->SetLabel("Ocean_Omega", TOP|CENTER, 3);
		hor1->Widget->SetRange( 0.0f, 1.0f);
		hor1->Widget->SetIncrement(.05f);
		hor1->Widget->SetLength(200);
		hor1->Widget->SetLink(&DemoControl.Ocean.Omega);
		hor1->Align_To_Widget(hor, BOTTOM |IN_LINE, 5);

	SetPos(100, 100);
	
}
void cGraphicsWindow::ReloadShaders(){
	//ReloadShaders();
}
cGraphicsWindow::~cGraphicsWindow() {// this is to clear the pointer in democontrol so it knows the control is gone
	self=0;
}