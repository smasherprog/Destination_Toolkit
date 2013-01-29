#include "cParticle.h"
#include "cSpring.h"

cSpring::cSpring(Particle *p1, Particle *p2, float restLength)
{
	m_pParticle1 = p1;
	m_pParticle2 = p2;
	m_restLength = restLength;

	SatisfyConstraints(std::vector<OBB*>());
	p1->m_oldPos = p1->m_oldPos = p1->m_pos;
	p2->m_oldPos = p2->m_oldPos = p2->m_pos;
}

void cSpring::Render(){
	
}

void cSpring::SatisfyConstraints(std::vector<OBB*> &obstacles)
{
	vec3 delta = m_pParticle1->m_pos - m_pParticle2->m_pos;
	float Length = delta.Length();
	float diff = (Length-m_restLength)/Length;
	m_pParticle1->m_pos -= delta * 0.5f * diff;
	m_pParticle2->m_pos += delta * 0.5f * diff;
}