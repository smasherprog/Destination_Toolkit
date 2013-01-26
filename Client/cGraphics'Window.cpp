#include "stdafx.h"
/*
#include "Gwen/Controls/TabControl.h"
#include "Gwen/Controls/CheckBox.h"

#include "Gwen/Controls/GroupBox.h"
#include "Gwen/Controls/ComboBox.h"

GWEN_CONTROL_CONSTRUCTOR( cGraphicsWindow  ){
	SetTitle("Graphics Options");
	SetSize(500, 600);
	SetPos(100, 100);
	
	Gwen::Controls::TabControl* m_TabControl = new Gwen::Controls::TabControl( this );
	m_TabControl->Dock( Gwen::Pos::Fill );
	m_TabControl->SetMargin( Gwen::Margin( 2, 2, 2, 2 ) );

	Gwen::Controls::TabButton* pButton = m_TabControl->AddPage( L"Drawing Options" );
	Gwen::Controls::Base* pPage = pButton->GetPage();

	Gwen::Controls::ScrollControl* pCtrl = new Gwen::Controls::ScrollControl( pPage );
	pCtrl->Dock(Gwen::Pos::Fill);
	

	Gwen::Controls::GroupBox* pGroup = new Gwen::Controls::GroupBox( pPage );
	pGroup->SetText( "General Options" );
	pGroup->SetPos(6, 6);
	pGroup->SetSize( 220, 290 );

		Gwen::Controls::CheckBoxWithLabel* labeled = new Gwen::Controls::CheckBoxWithLabel( pGroup );
		labeled->SetPos( 2, 10 );
		labeled->Label()->SetText( "Use FXAA(Blurs Edges)" );
		labeled->Checkbox()->SetChecked(Renderer.DemoControl.Use_FXAA);
		labeled->Checkbox()->onCheckChanged.Add( this, &cGraphicsWindow::FXAAChanged );

		/////////label
		Gwen::Controls::Label* Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 32);
		Label->SetText("Level Of Detail 1 Draw Distance");
		Label->SizeToContents();

		///////slider
		LodDrawDist1Slider = new Gwen::Controls::HorizontalSlider( pGroup );
		LodDrawDist1Slider->SetPos(2, 52);
		LodDrawDist1Slider->SetSize( 100, 20 );
		LodDrawDist1Slider->SetRange( 100, 1000 );
		LodDrawDist1Slider->SetValue( Renderer.DemoControl.ObjectContainer.Lod1Dist);
		LodDrawDist1Slider->onValueChanged.Add( this, &cGraphicsWindow::Lod1Slider );
		///slider numeric value
		LodDrawDist1Text = new Gwen::Controls::Label(pGroup);
		LodDrawDist1Text->SetPos(4 + 100, 52);
		//Init slider
		Lod1Slider(LodDrawDist1Slider);
/////////CONTROL
		/////////label
		Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 72);
		Label->SetText("Level Of Detail 2 Draw Distance");
		Label->SizeToContents();

		///////slider
		LodDrawDist2Slider = new Gwen::Controls::HorizontalSlider( pGroup );
		LodDrawDist2Slider->SetPos(2, 92);
		LodDrawDist2Slider->SetSize( 100, 20 );
		LodDrawDist2Slider->SetRange( 1001, 5000);
		LodDrawDist2Slider->SetValue( Renderer.DemoControl.ObjectContainer.Lod2Dist);
		LodDrawDist2Slider->onValueChanged.Add( this, &cGraphicsWindow::Lod2Slider );
		///slider numeric value
		LodDrawDist2Text = new Gwen::Controls::Label(pGroup);
		LodDrawDist2Text->SetPos(4 + 100, 92);
		//Init slider
		Lod2Slider(LodDrawDist2Slider);
//////// CONTROL
				/////////label
		Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 112);
		Label->SetText("Camera Near Plane");
		Label->SizeToContents();

		///////slider
		CameraNearPlaneSlider = new Gwen::Controls::HorizontalSlider( pGroup );
		CameraNearPlaneSlider->SetPos(2, 132);
		CameraNearPlaneSlider->SetSize( 100, 20 );
		CameraNearPlaneSlider->SetRange( 2, 50);
		CameraNearPlaneSlider->SetValue( Camera.NearPlane);
		CameraNearPlaneSlider->onValueChanged.Add( this, &cGraphicsWindow::CameraNearSlider );
		///slider numeric value
		CameraNearPlaneText = new Gwen::Controls::Label(pGroup);
		CameraNearPlaneText->SetPos(4 + 100, 132);
		//Init slider
		CameraNearSlider(CameraNearPlaneSlider);

		
//////// CONTROL
				/////////label
		Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 152);
		Label->SetText("Camera Far Plane");
		Label->SizeToContents();

		///////slider
		CameraFarPlaneSlider = new Gwen::Controls::HorizontalSlider( pGroup );
		CameraFarPlaneSlider->SetPos(2, 172);
		CameraFarPlaneSlider->SetSize( 100, 20 );
		CameraFarPlaneSlider->SetRange( 51, 10000);
		CameraFarPlaneSlider->SetValue( Camera.FarPlane);
		CameraFarPlaneSlider->onValueChanged.Add( this, &cGraphicsWindow::CameraFarSlider );
		///slider numeric value
		CameraFarPlaneText = new Gwen::Controls::Label(pGroup);
		CameraFarPlaneText->SetPos(4 + 100, 172);
		//Init slider
		CameraFarSlider(CameraFarPlaneSlider);
		
		Gwen::Controls::Label* boxlabel = new Gwen::Controls::Label(pGroup);
		boxlabel->SetPos(2, 192);
		boxlabel->SetText("Visual Debugging");
		boxlabel->SizeToContents();

		Gwen::Controls::ComboBox* combo = new Gwen::Controls::ComboBox( pGroup );
		combo->SetPos(2, 212);
		combo->SetWidth( 100 );
		combo->AddItem( L"No_Debug", "9" );
		combo->AddItem( L"Show_Normals", "0" );
		combo->AddItem( L"Show_Depth", "1" );
		combo->AddItem( L"Show_Diffuse", "2" );
		combo->AddItem( L"Show_Shadow", "3" );
		combo->AddItem( L"Show_Specular", "4" );
		combo->AddItem( L"Show_Stencil", "5" );
		combo->onSelection.Add( this, &cGraphicsWindow::DebugOptions );

		Gwen::Controls::Button* btn = new Gwen::Controls::Button( pGroup );
		btn->SetPos(2, 240);
		btn->SetText("Reload Shaders");
		btn->onPress.Add(this, &cGraphicsWindow::ReloadShaders );

		labeled = new Gwen::Controls::CheckBoxWithLabel( pGroup );
		labeled->SetPos( 2, 258);
		labeled->Label()->SetText( "Show Lights" );
		labeled->Checkbox()->SetChecked(Renderer.DemoControl.ObjectContainer.ShowPointLights);
		labeled->Checkbox()->onCheckChanged.Add( this, &cGraphicsWindow::ShowLightsChanged );

		pGroup = new Gwen::Controls::GroupBox( pPage );
		pGroup->SetText( "Shadowing Options" );
		pGroup->SetPos(6 + 220, 6);
		pGroup->SetSize( 220, 290 );

					/////////label
		labeled = new Gwen::Controls::CheckBoxWithLabel( pGroup );
		labeled->SetPos( 2, 10 );
		labeled->Label()->SetText( "Show_Shadow_Splits" );
		labeled->Checkbox()->SetChecked(Renderer.DemoControl.GlobalLight.ShowSplits);
		labeled->Checkbox()->onCheckChanged.Add( this, &cGraphicsWindow::Show_Splits );

//////// CONTROL
				/////////label
		Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 30);
		Label->SetText("Projection_Size");
		Label->SizeToContents();

		///////slider
		Projection_SizeSlider = new Gwen::Controls::HorizontalSlider( pGroup );
		Projection_SizeSlider->SetPos(2, 50);
		Projection_SizeSlider->SetSize( 100, 20 );
		Projection_SizeSlider->SetRange( 500, 10000);
		Projection_SizeSlider->SetValue( Renderer.DemoControl.GlobalLight.ProjSize);
		Projection_SizeSlider->onValueChanged.Add( this, &cGraphicsWindow::ProjectSlider );
		///slider numeric value
		Projection_SizeText = new Gwen::Controls::Label(pGroup);
		Projection_SizeText->SetPos(4 + 100, 50);
		//Init slider
		ProjectSlider(Projection_SizeSlider);
			
//////// CONTROL

		Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 70);
		Label->SetText("ShadowMap Distance");
		Label->SizeToContents();

		///////slider
		ShadowMapDistanceSlider = new Gwen::Controls::HorizontalSlider( pGroup );
		ShadowMapDistanceSlider->SetPos(2, 90);
		ShadowMapDistanceSlider->SetSize( 100, 20 );
		ShadowMapDistanceSlider->SetRange( 0, 10000);
		ShadowMapDistanceSlider->SetValue( Renderer.DemoControl.GlobalLight.ShadowMapDistance);
		ShadowMapDistanceSlider->onValueChanged.Add( this, &cGraphicsWindow::ShadowDistanceSlider );
		///slider numeric value
		ShadowMapDistanceText = new Gwen::Controls::Label(pGroup);
		ShadowMapDistanceText->SetPos(4 + 100, 90);
		//Init slider
		ShadowDistanceSlider(ShadowMapDistanceSlider);
		
//////// CONTROL
				/////////label
		Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 110);
		Label->SetText("Lambda Split");
		Label->SizeToContents();

		///////slider
		LambdaSlider = new Gwen::Controls::HorizontalSlider( pGroup );
		LambdaSlider->SetPos(2, 130);
		LambdaSlider->SetSize( 100, 20 );
		LambdaSlider->SetRange( .3f, 1.0f);
		LambdaSlider->SetValue( Renderer.DemoControl.GlobalLight.Lambda);
		LambdaSlider->onValueChanged.Add( this, &cGraphicsWindow::LambdaSplitSlider );
		///slider numeric value
		LambdaText = new Gwen::Controls::Label(pGroup);
		LambdaText->SetPos(4 + 100, 130);
		//Init slider
		LambdaSplitSlider(LambdaSlider);

		boxlabel = new Gwen::Controls::Label(pGroup);
		boxlabel->SetPos(2, 150);
		boxlabel->SetText("Shadow Map Size");
		boxlabel->SizeToContents();

		combo = new Gwen::Controls::ComboBox( pGroup );
		combo->SetPos(2, 170);
		combo->SetWidth( 100 );
		combo->AddItem( L"512x512", "0" );
		combo->AddItem( L"1024x1024", "1" );
		combo->AddItem( L"2048x2048", "2" );
		
		combo->onSelection.Add( this, &cGraphicsWindow::ShadowMapSize );

		boxlabel = new Gwen::Controls::Label(pGroup);
		boxlabel->SetPos(2, 190);
		boxlabel->SetText("Blur Samples");
		boxlabel->SizeToContents();

		combo = new Gwen::Controls::ComboBox( pGroup );
		combo->SetPos(2, 210);
		combo->SetWidth( 100 );
		combo->AddItem( L"1x1", "0" );
		combo->AddItem( L"3x3", "1" );
		combo->AddItem( L"5x5", "2" );
		combo->AddItem( L"7x7", "3" );
		combo->AddItem( L"9x9", "4" );
		combo->AddItem( L"11x11", "5" );
		combo->AddItem( L"13x13", "6" );

		combo->onSelection.Add( this, &cGraphicsWindow::ShadowMapBlur );

		pGroup = new Gwen::Controls::GroupBox( pPage );
		pGroup->SetText( "Weather Options" );
		pGroup->SetPos(6 , 296);
		pGroup->SetSize( 220, 290 );

		boxlabel = new Gwen::Controls::Label(pGroup);
		boxlabel->SetPos(2, 10);
		boxlabel->SetText("Weather Effect");
		boxlabel->SizeToContents();

		combo = new Gwen::Controls::ComboBox( pGroup );
		combo->SetPos(2, 30);
		combo->SetWidth( 100 );
		combo->AddItem( L"No_Weather", "0" );
		combo->AddItem( L"Heavy_Rain", "1" );
		combo->AddItem( L"Light_Rain", "2" );
		combo->AddItem( L"Light_Snow", "3" );
		combo->AddItem( L"Heavy_Snow", "4" );
	
		combo->onSelection.Add( this, &cGraphicsWindow::WeatherChange );

				/////////label
		Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 50);
		Label->SetText("Cloud Cover");
		Label->SizeToContents();

		///////slider
		CloudCoverSlider = new Gwen::Controls::HorizontalSlider( pGroup );
		CloudCoverSlider->SetPos(2, 70);
		CloudCoverSlider->SetSize( 100, 20 );
		CloudCoverSlider->SetRange( 0.0f, 1.0f);
		CloudCoverSlider->SetValue( Renderer.DemoControl.Clouds.m_fCloudCover);
		CloudCoverSlider->onValueChanged.Add( this, &cGraphicsWindow::CloudCoverChangeSlider );
		///slider numeric value
		CloudCoverText = new Gwen::Controls::Label(pGroup);
		CloudCoverText->SetPos(4 + 100, 70);
		//Init slider
		CloudCoverChangeSlider(CloudCoverSlider);


						/////////label
		Label = new Gwen::Controls::Label(pGroup);
		Label->SetPos(2, 90);
		Label->SetText("Sun Speed");
		Label->SizeToContents();

		///////slider
		SunSpeedSlider = new Gwen::Controls::HorizontalSlider( pGroup );
		SunSpeedSlider->SetPos(2, 110);
		SunSpeedSlider->SetSize( 100, 20 );
		SunSpeedSlider->SetRange( 0.0f, .5f);
		SunSpeedSlider->SetValue( Renderer.DemoControl.SunSpeed);
		SunSpeedSlider->onValueChanged.Add( this, &cGraphicsWindow::SunSpeedChanged );
		///slider numeric value
		SunSpeedText = new Gwen::Controls::Label(pGroup);
		SunSpeedText->SetPos(4 + 100, 110);
		//Init slider
		SunSpeedChanged(SunSpeedSlider);

	
	//TwAddVarRW(EditGraphics, "Ocean_hdr_Exposure",	TW_TYPE_FLOAT, &Ocean.hdrExposure ,				" Min=0 Max=1 Step=.05");
	//TwAddVarRW(EditGraphics, "Ocean_Wind",			TW_TYPE_FLOAT, &Ocean.Wind,						" Min=1 Max=25 Step=1");
	//TwAddVarRW(EditGraphics, "Ocean_Omega",			TW_TYPE_FLOAT, &Ocean.Omega,					" Min=0 Max=1 Step=.05");


	
}
void cGraphicsWindow::SetHidden(bool hidden){
	Renderer.DemoControl.GraphicsWindow=0;
	WindowControl::SetHidden(hidden);
}
void cGraphicsWindow::FXAAChanged( Gwen::Controls::Base* pControl ){
	if(Renderer.DemoControl.Use_FXAA) Renderer.DemoControl.Use_FXAA=false;
	else Renderer.DemoControl.Use_FXAA = true;
}
void cGraphicsWindow::ShowLightsChanged( Gwen::Controls::Base* pControl ){
	if(Renderer.DemoControl.ObjectContainer.ShowPointLights) Renderer.DemoControl.ObjectContainer.ShowPointLights=false;
	else Renderer.DemoControl.ObjectContainer.ShowPointLights = true;
}
void cGraphicsWindow::Show_Splits( Gwen::Controls::Base* pControl ){
	if(Renderer.DemoControl.GlobalLight.ShowSplits) Renderer.DemoControl.GlobalLight.ShowSplits=false;
	else Renderer.DemoControl.GlobalLight.ShowSplits = true;
}

void cGraphicsWindow::Lod1Slider( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Renderer.DemoControl.ObjectContainer.Lod1Dist = pSlider->GetValue();
	ostringstream ok;
	ok<<Renderer.DemoControl.ObjectContainer.Lod1Dist;
	LodDrawDist1Text->SetText(ok.str());
	LodDrawDist1Text->SizeToContents();

	if(Renderer.DemoControl.ObjectContainer.Lod1Dist > Renderer.DemoControl.ObjectContainer.Lod2Dist){
		LodDrawDist2Slider->SetValue(Renderer.DemoControl.ObjectContainer.Lod1Dist);
		Lod2Slider(LodDrawDist2Slider);
	}
}
void cGraphicsWindow::Lod2Slider( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Renderer.DemoControl.ObjectContainer.Lod2Dist = pSlider->GetValue();
	ostringstream ok;
	ok<<Renderer.DemoControl.ObjectContainer.Lod2Dist;
	LodDrawDist2Text->SetText(ok.str());
	LodDrawDist2Text->SizeToContents();

	if(Renderer.DemoControl.ObjectContainer.Lod2Dist < Renderer.DemoControl.ObjectContainer.Lod1Dist+1){
		LodDrawDist1Slider->SetValue(Renderer.DemoControl.ObjectContainer.Lod2Dist-1.0f);
		Lod1Slider(LodDrawDist1Slider);
	}
}
void cGraphicsWindow::CameraNearSlider( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Camera.NearPlane = pSlider->GetValue();
	ostringstream ok;
	ok<<Camera.NearPlane;
	CameraNearPlaneText->SetText(ok.str());
	CameraNearPlaneText->SizeToContents();

	if(Camera.NearPlane + 1 > Camera.FarPlane){// set the far plane +1 off the near plane
		CameraFarPlaneSlider->SetValue(Camera.NearPlane+1);
		CameraFarSlider(CameraFarPlaneSlider);
	}
}
void cGraphicsWindow::CameraFarSlider( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Camera.FarPlane = pSlider->GetValue();
	ostringstream ok;
	ok<<Camera.FarPlane;
	CameraFarPlaneText->SetText(ok.str());
	CameraFarPlaneText->SizeToContents();

	if(Camera.FarPlane < Camera.NearPlane +1 ){// set the far plane +1 off the near plane
		CameraNearPlaneSlider->SetValue(Camera.FarPlane-1);
		CameraNearSlider(CameraFarPlaneSlider);
	}
}

void cGraphicsWindow::DebugOptions( Gwen::Controls::Base* pControl ){
	Gwen::Controls::ComboBox* combo = (Gwen::Controls::ComboBox*)pControl;
	char f=combo->GetSelectedItem()->GetName()[0];// only need one character

	switch(f){
		case('0'):
			Renderer.DemoControl.DebugTech = Renderer.DemoControl.NormalsTech;
			break;
		case('1'):
			Renderer.DemoControl.DebugTech = Renderer.DemoControl.DepthTech;
			break;
		case('2'):
			Renderer.DemoControl.DebugTech = Renderer.DemoControl.DiffuseTech;
			break;
		case('3'):
			Renderer.DemoControl.DebugTech = Renderer.DemoControl.ShadowTech;
			break;
		case('4'):
			Renderer.DemoControl.DebugTech = Renderer.DemoControl.SpecularTech;
			break;
		case('5'):
			Renderer.DemoControl.DebugTech = Renderer.DemoControl.StencilTech;
			break;
		case('9'):
			Renderer.DemoControl.DebugTech = -1;
			break;
	};
}
void cGraphicsWindow::ReloadShaders( Gwen::Controls::Base* pControl ){
	Renderer.ReloadShaders();
}

void cGraphicsWindow::ProjectSlider( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Renderer.DemoControl.GlobalLight.ProjSize = pSlider->GetValue();
	ostringstream ok;
	ok<<Renderer.DemoControl.GlobalLight.ProjSize;
	Projection_SizeText->SetText(ok.str());
	Projection_SizeText->SizeToContents();
}
void cGraphicsWindow::ShadowDistanceSlider( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Renderer.DemoControl.GlobalLight.ShadowMapDistance = pSlider->GetValue();
	ostringstream ok;
	ok<<Renderer.DemoControl.GlobalLight.ShadowMapDistance;
	ShadowMapDistanceText->SetText(ok.str());
	ShadowMapDistanceText->SizeToContents();
}
void cGraphicsWindow::LambdaSplitSlider( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Renderer.DemoControl.GlobalLight.Lambda = pSlider->GetValue();
	ostringstream ok;
	ok<<Renderer.DemoControl.GlobalLight.Lambda;
	LambdaText->SetText(ok.str());
	LambdaText->SizeToContents();
}
void cGraphicsWindow::ShadowMapSize( Gwen::Controls::Base* pControl ){
	Gwen::Controls::ComboBox* combo = (Gwen::Controls::ComboBox*)pControl;
	char f=combo->GetSelectedItem()->GetName()[0];// only need one character
	switch(f){
		case('0'):
			Renderer.DemoControl.GlobalLight.ChangeShadowMap = 512;
			break;
		case('1'):
			Renderer.DemoControl.GlobalLight.ChangeShadowMap = 1024;
			break;
		case('2'):
			Renderer.DemoControl.GlobalLight.ChangeShadowMap = 2048;
			break;
	};
}
void cGraphicsWindow::ShadowMapBlur( Gwen::Controls::Base* pControl ){
	Gwen::Controls::ComboBox* combo = (Gwen::Controls::ComboBox*)pControl;
	char f=combo->GetSelectedItem()->GetName()[0];// only need one character

	switch(f){
		case('0'):
			Renderer.DemoControl.GlobalLight.BlurSamples = 1;
			break;
		case('1'):
			Renderer.DemoControl.GlobalLight.BlurSamples = 3;
			break;
		case('2'):
			Renderer.DemoControl.GlobalLight.BlurSamples = 5;
			break;
		case('3'):
			Renderer.DemoControl.GlobalLight.BlurSamples = 7;
			break;
		case('4'):
			Renderer.DemoControl.GlobalLight.BlurSamples = 9;
			break;
		case('5'):
			Renderer.DemoControl.GlobalLight.BlurSamples = 11;
			break;
		case('6'):
			Renderer.DemoControl.GlobalLight.BlurSamples = 13;
			break;
	};
}
void cGraphicsWindow::WeatherChange( Gwen::Controls::Base* pControl ){
	Gwen::Controls::ComboBox* combo = (Gwen::Controls::ComboBox*)pControl;
	char f=combo->GetSelectedItem()->GetName()[0];// only need one character

	switch(f){
		case('0'):
			Renderer.DemoControl.Weather.BlendTo = 0;
			break;
		case('1'):
			Renderer.DemoControl.Weather.BlendTo = 1;
			break;
		case('2'):
			Renderer.DemoControl.Weather.BlendTo = 2;
			break;
		case('3'):
			Renderer.DemoControl.Weather.BlendTo = 3;
			break;
		case('4'):
			Renderer.DemoControl.Weather.BlendTo = 4;
			break;
	};
}
void cGraphicsWindow::CloudCoverChangeSlider( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Renderer.DemoControl.Clouds.m_fCloudCover = pSlider->GetValue();
	ostringstream ok;
	ok<<Renderer.DemoControl.Clouds.m_fCloudCover;
	CloudCoverText->SetText(ok.str());
	CloudCoverText->SizeToContents();
}
void cGraphicsWindow::SunSpeedChanged( Gwen::Controls::Base* pControl ){
	Gwen::Controls::HorizontalSlider* pSlider = (Gwen::Controls::HorizontalSlider*)pControl;
	Renderer.DemoControl.SunSpeed = pSlider->GetValue();
	ostringstream ok;
	ok<<Renderer.DemoControl.SunSpeed;
	SunSpeedText->SetText(ok.str());
	SunSpeedText->SizeToContents();
}
*/