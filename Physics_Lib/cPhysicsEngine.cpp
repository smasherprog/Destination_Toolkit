#include "cPhysicsEngine.h"
#include "cParticle.h"
#include "../BV_Lib/cOBB.h"
#include "../Utilities/MyMath.h"
#include "../Utilities/My_Timer.h"
#include "cSpring.h"

void cPhysicsEngine::Init(){
	Rand_Generator.seed(Rand_Seeder());
	Reset();
	
	//Add Obstacles
	
	//cOBB *obb = new cOBB(vec3(5.0f, 0.2f, 5.0f));
	//obb->Center = vec3(0.0f, -1.0f, 0.0f) + vec3(-150, 85, 235);
	//obb->Init();
	//m_obstacles.push_back(obb);
	
	cOBB* obb = new cOBB(vec3(0.8f, 0.1f, 2.0f));
	obb->Center = vec3(-0.5f, 1.0f, 0.0f)  + vec3(-150, 85, 235);
	obb->Rot.SetupRotation(euler(0.0f, -0.1f, -0.5f));
	obb->Init();
	m_obstacles.push_back(obb);

	obb = new cOBB(vec3(0.8f, 0.1f, 2.0f));
	obb->Center = vec3(0.5f, 2.0f, 0.0f)  + vec3(-150, 85, 235);
	obb->Rot.SetupRotation(euler( 0.1f, 0.0f, 0.4f));
	obb->Init();
	m_obstacles.push_back(obb);

	obb = new cOBB(vec3(1.0f, 0.1f, 2.0f));
	obb->Center = vec3(-0.5f, 3.0f, 0.0f)  + vec3(-150, 85, 235);
	obb->Rot.SetupRotation(euler( 0.0f, -0.1f, -0.3f));
	obb->Init();
	m_obstacles.push_back(obb);

	obb = new cOBB(vec3(0.5f, 0.1f, 1.0f));
	obb->Center = vec3(0.8f, 4.0f, 0.0f)  + vec3(-150, 85, 235);
	obb->Rot.SetupRotation(euler(0.1f, 0.0f, 0.2f));
	obb->Init();
	m_obstacles.push_back(obb);
}
cPhysicsEngine::~cPhysicsEngine(){
	DeInit();
}
void cPhysicsEngine::DeInit(){
	for(int i=0; i<(int)m_physicsObjects.size(); i++) delete m_physicsObjects[i];
	m_physicsObjects.clear();
	for(size_t i=0; i<m_obstacles.size(); i++) delete m_obstacles[i];
	m_obstacles.clear();
}
void cPhysicsEngine::Reset(){//Remove old objects
	for(int i=0; i<(int)m_physicsObjects.size(); i++) delete m_physicsObjects[i];
	m_physicsObjects.clear();

	//Add new Particles
	for(int i=0; i<100; i++){
		float x = ((Rand_Generator()%1000) / 1000.0f) * 3.0f - 1.5f;
		float z = ((Rand_Generator()%1000) / 1000.0f) * 3.0f - 1.5f;
		float y = ((Rand_Generator()%1000) / 1000.0f) * 4.0f + 5.0f;
		m_physicsObjects.push_back(new Particle(vec3(x, y, z)+ vec3(-150, 85, 235)));
	}
		//Add Springs
	//int numParticles = (int)m_physicsObjects.size();
	//for(int i=0; i<numParticles-1; i+=2){
	//	m_physicsObjects.push_back(new cSpring((Particle*)m_physicsObjects[i], (Particle*)m_physicsObjects[i + 1],  1.0f));
	//}

}

void cPhysicsEngine::Update(float deltaTime){
	//if(ShiftDown){
		//Reset();
	//}
	//For safety allow max 3 timesteps
	m_time = min(m_time + deltaTime, TIME_STEP * 3.0f);
	while(m_time > TIME_STEP){
		AddForces();
		//Update Objects
		for(size_t i=0; i<m_physicsObjects.size(); i++)
			m_physicsObjects[i]->Update(TIME_STEP);
		SatisfyConstraints();
		m_time -= TIME_STEP;
	}
}

void cPhysicsEngine::AddForces(){//Update forces
	for(size_t i=0; i<m_physicsObjects.size(); i++){
		m_physicsObjects[i]->AddForces();
	}
}

void cPhysicsEngine::SatisfyConstraints(){
	//Satisfy Constraints
	int numObj = (int)m_physicsObjects.size();

	for(int i=numObj - 1; i >= 0; i--) {
		m_physicsObjects[i]->SatisfyConstraints(m_obstacles);
	}
}

void cPhysicsEngine::Render(){
	//Render Objects
	for(size_t i=0; i<m_physicsObjects.size(); i++)
		m_physicsObjects[i]->Render();
	
	//Render Obstacles
	//for(int i=0; i<m_obstacles.size(); i++)
		//m_obstacles[i]->Draw(0);
	
}