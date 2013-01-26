#ifndef CDEMOCONTROL_H
#define CDEMOCONTROL_H


#include "cSky.h"
#include "cGlobalLight.h"
#include "cObjectContainer.h"
#include "../Common_Game_Objects_Lib/cCharacter.h"
#include "cClouds.h"
#include "WeatherEffects.h"
#include "cOcean.h"
#include "../Physics_Lib/cPhysicsEngine.h"
#include "cPointLight.h"
#include "Network_DemoControl.h"
#include "../MY_UI\cWidgetEvents.h"
#include "../Graphics_Lib\Post_Pass.h"
#include "Terrain.h"
#include "../Graphics_Lib/FXAA.h"

//forward delcarations
class cMesh;

// forward delclations
namespace MY_UI{
	namespace Controls {
		class Text;
		class Collapsible_List;
		
	};
};

#define CHATMESSAGEBUFFER 40
struct cChat_Messages_ForUI{
	char index;
	std::string label;
};

class cDemoControl: public Network_DemoControl {
public:

	unsigned int Focus;

	cPhysicsEngine PhysicsEngine;
	//cTextureViewer TextureViewer;
	Post_Pass PostPass;
	cOcean Ocean;
	DynamicWeather Weather;
	FXAABlender FXAA;
	Terrain Terr;
	//cLand Land;
	cSky Sky;
	CCloud Clouds;

	cGlobalLight GlobalLight;
	MY_UI::Controls::Text* TargetText;

	bool FreeFlying;

	cObjectContainer ObjectContainer;

	MY_UI::Controls::Collapsible_List* LeftMenu;
	unsigned int Onresizeid;

	cDemoControl();
	virtual ~cDemoControl();


	void DeInit();
	void Init(uint32_t gameserverip, const std::string& login, uint8_t* key);

	void Update();
	void Run();
	void OnResize(unsigned int width, unsigned int height);

	void SetTarget(cBaseMesh* target);
	cBaseMesh* GetTarget();
	cBaseMesh* LoadObject(const std::string filename, const vec3 pos = vec3(0.0f, 0.0f, 0.0f), const std::string name = "The Unamed!", const uint32_t type = NULL_TYPE);
	
	void Enter_World();
	void Create_Character(std::string name);

	void ConnectToGameServer();
	void WorldUpdate(char* data, unsigned int length, size_t channel);
	void WorldState(char* data, unsigned int length, size_t channel);
	void WorldChanges(char* data, unsigned int length, size_t channel);

	void UpdateCharacterState(CharacterChange& cc);
	void AddCharacterToWorld(char* beg);
	void RemoveCharacterFromWorld(char* beg);

};

#endif