#ifndef CSPRING_H
#define CSPRING_H

#include "cPhysicsEngine.h"
#include "cParticle.h"
#include "../BV_Lib/cSphere.h"

using namespace std;
class OBB;

class cSpring : public cPhysicsObject{
public:
	cSpring(Particle *p1, Particle *p2, float restLength);
	void Update(float deltaTime){}
	void Render();
	void AddForces(){}
	void SatisfyConstraints(std::vector<OBB*> &obstacles);

	Particle *m_pParticle1;
	Particle *m_pParticle2;
	float m_restLength;
};

#endif