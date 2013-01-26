#ifndef PHYSICS_PARTICLE
#define PHYSICS_PARTICLE

#include "../Utilities/MyMath.h"
#include "cPhysicsEngine.h"
#include "../BV_Lib/cSphere.h"

using namespace std;
class cOBB;

class Particle : public cPhysicsObject
{
	friend class Spring;
	public:
		Particle();
		Particle(vec3 pos);
		void Update(float deltaTime);
		void Render();
		void AddForces();
		void SatisfyConstraints(std::vector<cOBB*> &obstacles);

		cSphere Sphere;
		float m_bounce;
};

#endif