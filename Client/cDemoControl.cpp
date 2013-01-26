#include "stdafx.h"
#include "cDemoControl.h"
#include "../Graphics_Lib/cMesh.h"
#include "cTerrainWindow.h"
#include "cCharacterSelection.h"
#include "../MY_UI\Collapsible_List.h"
#include "cObjectWindow.h"
#include "cGraphicsWindow.h"
#include "cChatWindow.h"
#include "UI_Camera.h"
#include "../MY_UI/ApplicationLayer.h"
#include "UI_Ocean.h"
#include "Global_GameData.h"
#include "UI_Terrain.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include "cLoginControl.h"

///////////////////////////////////////////////////////////////////////
cDemoControl::~cDemoControl(){
	Graphics::UnRegisterForOnResize(Onresizeid);
} 
cDemoControl::cDemoControl(){ 
	Onresizeid= Graphics::RegisterForOnResize(std::bind(&cDemoControl::OnResize, this, std::placeholders::_1, std::placeholders::_2));
	TargetText=0;
	ActiveCharacter=0;// just in case
	Focus=Stopped;
	//ui Stuff
	LeftMenu=0;
	Prep_Function_Buffer(Network::GS_MaxFunctions);

	Register_Function([](char* , unsigned int, size_t){OUTPUT_DEBUG_MSG("Succesfully connected to the Game Server"); }, Network::GS_LOGIN_SUCCESS);
	Register_Function([this](char* , unsigned int, size_t){ 	
		MY_UI::Safe_Delete(cCharacterSelection::self);
		cCharacterSelection::self = new cCharacterSelection(MY_UI::Internal::RootWidget, this); 
	}, Network::GS_CHARACTER_LISTING);
	Register_Function([this](char* data, unsigned int length, size_t channel){ 	
		ActiveCharacter = *data++;
		Characters[ActiveCharacter]->Gid = PlayerID = *reinterpret_cast<uint16_t*>(data);
		Enter_World();
	}, Network::GS_ENTER_WORLD);
	Register_Function([this](char* data, unsigned int length, size_t channel){ 	
		uint16_t numofchars = *reinterpret_cast<uint16_t*>(data);
		data+=sizeof(uint16_t);
		//OUTPUT_DEBUG_MSG(" Received World update. Number of characters in update "<<numofchars);
		for(uint16_t i(0); i< numofchars; i++){	
			UpdateCharacterState(*reinterpret_cast<CharacterChange*>(data));
			data+=sizeof(CharacterChange);
		}
	}, Network::GS_WORLD_UPDATE);
	Register_Function([this](char* data, unsigned int length, size_t channel){ 	
		uint16_t numofchars = *reinterpret_cast<uint16_t*>(data);
		data+=sizeof(uint16_t);
		//OUTPUT_DEBUG_MSG("Received worldstate packet with "<<numofchars<<" Characters in it ");
		for(uint16_t i(0); i< numofchars; i++){
			AddCharacterToWorld(data);
		}
	}, Network::GS_WORLD_STATE);
	Register_Function([this](char* data, unsigned int length, size_t channel){ 	
		UpdateCharacterState(*reinterpret_cast<CharacterChange*>(data));
	}, Network::GS_CHARACTER_CHANGES);
	Register_Function([this](char* data, unsigned int length, size_t channel){ 	
		AddCharacterToWorld(data);
	}, Network::GS_CHARACTER_ENTERED_WORLD);
	Register_Function([this](char* data, unsigned int length, size_t channel){ 	
		RemoveCharacterFromWorld(data);
	}, Network::GS_CHARACTER_LEFT_WORLD);
	Register_Function(std::bind(&cOcean::Add_Ocean_Grid, Ocean, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		, Network::GS_CONF_OCEAN_GRID);
}
void cDemoControl::DeInit(){
	if(Focus == Stopped) return;
	OUTPUT_DEBUG_MSG("deInit on democontrol");

	for(std::map<uint16_t, CharacterAndMesh>::iterator beg(Players.begin()); beg!= Players.end(); beg++){
		if(!ObjectContainer.erase(beg->second.CharacterMesh.get())) OUTPUT_DEBUG_MSG("COULD NOT DELETE IT!!");
		// the mesh is deleted in the objectcontainer automatically
	}
	Players.clear();
	MyCharacter=0;
	MyCharacterMesh=0;

//////////// BEGIN handle UI shut down
	MY_UI::Safe_Delete(cCharacterSelection::self);
	MY_UI::Safe_Delete(cChatWindow::self);
	MY_UI::Safe_Delete(LeftMenu);
	MY_UI::Safe_Delete(cGraphicsWindow::self);
	MY_UI::Safe_Delete(cObjectWindow::self);
	MY_UI::Safe_Delete(TargetText);
	MY_UI::Safe_Delete(UI_Camera::self);
	MY_UI::Safe_Delete(Ocean_Window::self);
////////////END handle windows shut down	
	//TextureViewer.DeInit();
	ObjectContainer.DeInit();
	
	GlobalLight.DeInit();
	//Land.DeInit();
	Terr.DeInit();
	Clouds.DeInit();
	Weather.DeInit();
	Sky.DeInit();

	Ocean.DeInit();

	PhysicsEngine.DeInit();
	FXAA.DeInit();
	PostPass.DeInit();
	//Renderer.ReleaseShader(DebugShader);
	Focus = Stopped;
	Network_DemoControl::DeInit();
}
void cDemoControl::SetTarget(cBaseMesh* target){  ObjectContainer.SetTarget(target); }
cBaseMesh* cDemoControl::GetTarget(){ return ObjectContainer.GetTarget(); }	
cBaseMesh* cDemoControl::LoadObject(const std::string filename, const vec3 pos, const std::string name , const uint32_t type){ return ObjectContainer.LoadObject(filename, pos, name, type);  }

void cDemoControl::Init(uint32_t gameserverip, const std::string& login, uint8_t* key){
	OUTPUT_DEBUG_MSG("Init on democontrol");
	SunDir.normalize();
	Focus = Running;
	///UI INIT STUFF


	MY_UI::Safe_Delete(UI_Camera::self);
	UI_Camera::self = new UI_Camera(this);

	LeftMenu = new MY_UI::Controls::Collapsible_List(MY_UI::Internal::RootWidget);
	LeftMenu->SetSize( 100, 200 );
	LeftMenu->SetPos( 10, 200 );
	LeftMenu->Insert("Objects")->OnLeftClicked.Add([this]() {
		if(cObjectWindow::self ==0 ) cObjectWindow::self = new cObjectWindow(MY_UI::Internal::RootWidget, this);
	});
	LeftMenu->Insert("Graphics")->OnLeftClicked.Add([this]() {
		if(cGraphicsWindow::self ==0 ) cGraphicsWindow::self = new cGraphicsWindow(MY_UI::Internal::RootWidget, *this);
	});
	LeftMenu->Insert("Ocean Editor")->OnLeftClicked.Add([this](){
		if(Ocean_Window::self ==0 ) Ocean_Window::self = new Ocean_Window(MY_UI::Internal::RootWidget, this);
	});
	LeftMenu->Insert("Terrain Editor")->OnLeftClicked.Add([this]() {
		if(Terrain_Window::self ==0 ) Terrain_Window::self = new Terrain_Window(MY_UI::Internal::RootWidget, this);
	});


	for(size_t i(0); i<MAXCHARACTERS; i++) Characters[i] = make_shared<cCharacter>();
	
		
	Camera.Position = vec3(0, 300.0f, 0);// set the camera above the terrain
	float x, y;
	Graphics::GetBackBufferSize(x, y);
	Camera.setLens(.25f*Pi, y, x , 1.0f, 15000.0f);
	Camera.rebuildView(); 

	ObjectContainer.Init();
	GlobalLight.Init(1024, 5);// default settings, shadowmap size and blur samples
	//Land.Init();
	Terr.Init();
	//Camera.Position.y= Land.GetHeight(Camera.Position.x, Camera.Position.z)+4;
	Camera.OnResize(y, x); 
	//TextureViewer.OnResize();
	Ocean.Init();
	Clouds.Init();
	Weather.Init();
	FXAA.Init();
	PostPass.Init();
	Sky.Init();

	//PhysicsEngine.Init(&Land);

	TargetText = new MY_UI::Controls::Text(MY_UI::Internal::RootWidget);
	TargetText->SetText("Target:");
	TargetText->SetPos(300, 10);

	OUTPUT_DEBUG_MSG("Finished Init on DemoControl");
	for(float i = -256.0f; i != 256.0f; i+=64.0f){
		for(float j= -256.0f; j != 256.0f; j+=64.0f){
			vec3 tlpos(i, 0, j);
			if(Ocean.AddGrid(tlpos)){
				Ocean.GridAdded(tlpos);
			}
		}
	}
	// always put this at the end of the function so all the loading can finish
	Network_DemoControl::Init(gameserverip, login, key);
}

void cDemoControl::OnResize(unsigned int width, unsigned int height){ 
	Camera.OnResize((float)height, (float)width); 
} 
void cDemoControl::Update(){// do any updates here
	Camera.rebuildView();// update camera
	Run();
	cAABB aabb;
	aabb.Add(vec3(1000.0f, 1000.0f, 1000.0f ));
	aabb.Add(vec3(-1000.0f, -1000.0f, -1000.0f ));
	//Ocean.CullObjectsOutside(aabb);// update the ocean

	// update any characters
	for(std::map<uint16_t, CharacterAndMesh>::iterator beg(Players.begin()); beg!= Players.end(); beg++){
		beg->second.Character->Run();
		vec3 pos = beg->second.Character->Position;
		float height= 0.0f;//Land.GetHeight(pos.x, pos.z);
		beg->second.CharacterMesh->SetPosition(pos.x, height, pos.z);
		beg->second.CharacterMesh->SetRotationX(beg->second.Character->Look);
		beg->second.CharacterMesh->UpdateWorldMatrix();
	}
	Terr.Update();
}

void cDemoControl::Run(){
	if(Focus == Stopped) return;
	else if(Focus == ShuttingDown) return DeInit();
	Update();
	


	static vec3 lastsun(50, 50, 50), oldpos(-1233333, 0, -123);// seed this so the first loop through will build the shadow map

	static vec3 lastlook(45, 565656, 56);
	static float lasttime = 0;
	lasttime+=My_Timer::DT;
	/*
	if(lasttime >1.0f/10.0f){// cap the shadow updates to 10 per frame..
	// |(!CloseTo(lastsun, SunDir))	
		 if( (!CloseTo(Camera.Look,lastlook))  | (!CloseTo(oldpos, Camera.Position, 1) ) | (lasttime >1.0f) ){// only build a new map when it is needed!! mm kay?
			GlobalLight.PrepShadowMap(Land.GetGridHeights());
			Plane planes[6];
			for(int32_t i = 0; i < 4; i++){
				mat4* shadowvp =GlobalLight.ShadowMapPass(i);
				ExtractPlanesD3D(planes, *shadowvp, true);
				//ObjectContainer.Draw4ShadowMap(planes);
				Land.Draw4ShadowMap(planes);
			}
		
			oldpos=Camera.Position;
			lastlook=Camera.Look;
			lasttime =0;
			GlobalLight.BlurShadowMaps();

		}
	}
	*/
	//set the color, and the normal textures to be renderer to, also, bind the depth buffer		
	float col[4] = {1, 1, 1, 1};
	Graphics::AquireLock();
	Graphics::UnBindRenderTargets();
	Graphics::Texture* bufferRTs[] = { &Graphics::Textures::RT_BackBufferTexture, &Graphics::Textures::RT_Normal, &Graphics::Textures::RT_Depth };
	Graphics::BindRenderTargets(bufferRTs, 3, &Graphics::Textures::RT_DepthStencil);// set the render targets to base(aka color), normal and bind the depth texture to z checking can be done
	Graphics::Textures::RT_DepthStencil.ClearDepthTargets(true, true, 1.0f, 0);// clear the depth and set its value to 1.0f
	Graphics::Textures::RT_BackBufferTexture.ClearRenderTargets();
	Graphics::Textures::RT_Base.ClearRenderTargets();
	Graphics::ReleaseLock();
	///////////////DRAW EVERYTHING IN FRONT TO BACK TO HELP WITH DEPTH TESTING
	//ObjectContainer.DrawMesh();

	//Land.Draw();
	Graphics::AquireLock();
	Terr.Draw();
	Graphics::ReleaseLock();
	//PhysicsEngine.Update(cMyTime::DT);
	//PhysicsEngine.Render();
	Graphics::AquireLock();
	PostPass.Draw();
	Graphics::ReleaseLock();
	
	Graphics::AquireLock();
	Sky.Draw();
	Graphics::ReleaseLock();
	
	Graphics::AquireLock();
	Clouds.Draw();
	Graphics::ReleaseLock();
	
	//ObjectContainer.DrawLights();
	//Ocean.Draw();
	Graphics::AquireLock();
	Ocean.Draw1();
	Graphics::ReleaseLock();
	Graphics::AquireLock();
	Weather.Draw();
	Graphics::ReleaseLock();
	// make sure the UI is drawing to the backbuffer
	Graphics::AquireLock();
	FXAA.Blend(Graphics::Textures::RT_Base, Graphics::Textures::RT_BackBufferTexture);
	Graphics::ReleaseLock();
	//Land.DrawEditor();
	//TextureViewer.Draw();
	std::string targ = "Target: ";
	cBaseMesh* target = ObjectContainer.GetTarget();
	if(target) targ += target->GetName();
	TargetText->SetText(targ);

}
void cDemoControl::Enter_World(){
	vec3 pos = Characters[ActiveCharacter]->Position;
	//pos.y = Land.GetHeight(pos);
	cMesh* mesh = static_cast<cMesh*>(ObjectContainer.LoadObject(Asset_Dir + "Mesh\\RedKnightM.dsm", pos , Characters[ActiveCharacter]->Name, MESH));
	if(mesh){
		mesh->SetScaling(.1f, .1f, .1f);
		mesh->mAnimator.SetAnimation("Idle");	
		mesh->SetPosition(pos.x, pos.y, pos.z);
		mesh->UpdateWorldMatrix();
	} else {
		OUTPUT_DEBUG_MSG("Failed to load character!!!!!");
		return;
	}
	CharacterAndMesh candc;
	MyCharacter = candc.Character = Characters[ActiveCharacter];

	//MyCharacterMesh = candc.CharacterMesh = mesh;
	std::pair<std::map<uint16_t, CharacterAndMesh>::iterator, bool> ret = Players.insert(std::map<uint16_t, CharacterAndMesh>::value_type(PlayerID, candc));
	if(!ret.second) {
		OUTPUT_DEBUG_MSG("Failed to insert the player. This is not possible unless something is very wrong with this program. Expect a crash soon!!");
		ObjectContainer.erase(candc.CharacterMesh.get());
	}
	OUTPUT_DEBUG_MSG(" Character entered world. PlayerID is "<<PlayerID<<" Pos is "<<pos);
	if(cChatWindow::self ==0){
		new cChatWindow(MY_UI::Internal::RootWidget, this);
	} else OUTPUT_DEBUG_MSG("EERROOR, Chat window already allocated.......");
	cChatWindow::self->JoinCreateChannel("Default", "", Network::CHAT_JOIN_CHANNEL);

	MY_UI::Safe_Delete(cCharacterSelection::self);
}

void cDemoControl::UpdateCharacterState(CharacterChange& cc){

	std::map<uint16_t, CharacterAndMesh>::iterator found = Players.find(cc.gid);
	if(found == Players.end()) return;// dont know how this could happen, but I suppose it could

	cCharacter& c = *found->second.Character;	
	if(c.Gid == PlayerID){// found me, check my pos difference
		vec3 dif = c.Position - cc.pos;
		//	OUTPUT_DEBUG_MSG("Updateing mychar pos difference is "<<dif);
	} //else OUTPUT_DEBUG_MSG("Updateing "<<c.Name<<"'s internal state");
	c.Position = cc.pos;
	c.Look = cc.look;
	c.MovementState = cc.movementstate;
	uint8_t walk = c.GetForwardState();
	cMesh& mesh = *found->second.CharacterMesh;
	mesh.mAnimator.PlayAnimationForward();// just in case
	switch(walk){	
	case(WALK_FORWARD):
		mesh.mAnimator.SetAnimation("Walk");
		break;
	case(RUN_FORWARD):
		mesh.mAnimator.SetAnimation("Run");
		break;
	case(WALK_BACKWARD):
		mesh.mAnimator.PlayAnimationBackward();
		mesh.mAnimator.SetAnimation("Walk");
		break;
	default:
		mesh.mAnimator.SetAnimation("Idle");
	};

}


void cDemoControl::AddCharacterToWorld(char* beg){	
	std::shared_ptr<cCharacter> c = std::make_shared<cCharacter>();
	c->ConstructForVisual(beg);
	OUTPUT_DEBUG_MSG("Inserting the character with id "<<c->Gid);

	CharacterAndMesh candc;
	candc.Character = c;
	std::pair<std::map<uint16_t, CharacterAndMesh>::iterator, bool> ret = Players.insert(std::map<uint16_t, CharacterAndMesh>::value_type(c->Gid, candc));
	if(!ret.second) {
		OUTPUT_DEBUG_MSG("Failed to insert the player named "<<c->Name<<" With an id of: "<<c->Gid);
		if(ret.first->second.Character->Gid != PlayerID){
			OUTPUT_DEBUG_MSG("Failed to insert a character . . . .This is not possible unless something is very wrong with this program. Expect a crash soon!!");
		} 
		return;
	}
	OUTPUT_DEBUG_MSG("Inserting the character with id "<<c->Gid);
	vec3 pos = c->Position;
	//pos.y = Renderer.DemoControl.Land.GetHeight(pos);
	cMesh* mesh = static_cast<cMesh*>(ObjectContainer.LoadObject(Asset_Dir + "Mesh\\RedKnightM.dsm", pos , c->Name, MESH));

	if(mesh){
		mesh->SetScaling(.1f, .1f, .1f);
		mesh->mAnimator.SetAnimation("Idle");	
		mesh->SetPosition(pos.x, pos.y, pos.z);
		mesh->UpdateWorldMatrix();
	}
	//ret.first->second.CharacterMesh = mesh;	
}
void cDemoControl::RemoveCharacterFromWorld(char* beg){
	uint16_t gid = *reinterpret_cast<uint16_t*>(beg);
	beg+=sizeof(uint16_t);
	std::map<uint16_t, CharacterAndMesh>::iterator found = Players.find(gid);
	if(found ==Players.end()) return;// nothing to see here!!
	ObjectContainer.erase(found->second.CharacterMesh.get());// remove the mesh
	Players.erase(found);
}