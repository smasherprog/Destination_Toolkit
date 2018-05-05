#ifndef CPHYSICSENGINE
#define CPHYSICSENGINE
#include <vector>
#include "../Utilities/MyMath.h"
#include <random>


#pragma comment(lib, "legacy_stdio_definitions")

class OBB;
using namespace std;

#define GRAVITY  -9.82f
#define TIME_STEP  0.005f
#define DRAG  -0.3f
#define FRICTION  0.995f

class cPhysicsObject{// I hate virtual functions, but sometimes, they have their uses, this is one of them
public:
	vec3 m_pos, m_oldPos, m_forces;
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void AddForces() = 0;
	virtual void SatisfyConstraints(std::vector<OBB*> &obstacles) = 0;
};	

class cPhysicsEngine{
public:

	~cPhysicsEngine();
	void Init();
	void DeInit();

	void Update(float deltaTime);
	void AddForces();
	void SatisfyConstraints();
	void Render();
	void Reset();

	std::vector<cPhysicsObject*> m_physicsObjects;
	std::vector<OBB*> m_obstacles;
	float m_time;
	std::random_device Rand_Seeder;
	std::mt19937 Rand_Generator;

};


#endif