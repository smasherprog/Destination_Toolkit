#include "cParticle.h"
#include "../BV_Lib/cOBB.h"

Particle::Particle(){
	m_pos = vec3(0.0f, 0.0f, 0.0f);
	m_oldPos = vec3(0.0f, 0.0f, 0.0f);
	m_forces = vec3(0.0f, 0.0f, 0.0f);
	m_bounce = 0.7f;
}

Particle::Particle(vec3 pos){
	m_pos = pos;
	m_oldPos = pos;
	m_forces = vec3(0.0f, 0.0f, 0.0f);
	m_bounce = 0.5f + ((rand()%1000) / 1000.0f) * 0.2f;
}

void Particle::Update(float deltaTime){
	//Verlet Integration
	vec3 temp = m_pos;
    m_pos += (m_pos - m_oldPos) * FRICTION + m_forces * deltaTime * deltaTime;
	m_oldPos = temp;
}

void Particle::Render(){
	mat4 world, scale;
	scale.setupScale(.05f,.05f, .05f);
	world.setupTranslation(m_pos.x, m_pos.y + 0.025f, m_pos.z);
	//Sphere.Draw(scale*world);
}

void Particle::AddForces(){
	//Gravity
	m_forces = vec3(0.0f, GRAVITY, 0.0f);
}

void Particle::SatisfyConstraints(std::vector<cOBB*> &obstacles){
	for(size_t i=0; i<obstacles.size(); i++){
		if(obstacles[i]->Intersect(m_pos)){//Collision
			//Calculate Velocity
			vec3 V = m_pos - m_oldPos;
			vec3 dir=V;
			dir.normalize();
			vec3 org;
			obstacles[i]->GetContactPoint(dir, m_oldPos, dir, org);
			vec3 VN = Dot(dir, V) * dir;
			vec3 VT = V - VN;
			m_pos = org + 0.005f * dir;
			m_oldPos = m_pos - (VT - VN * m_bounce);
		}
	}
	/*
	if(land->Intersect(m_pos)){// check for land hit
		vec3 V = m_pos - m_oldPos;
		vec3 dir=V;
		dir.normalize();
		vec3 org;
		bool po = land->GetContactPoint(dir, m_oldPos, dir, org);

		vec3 VN = Dot(dir, V) * dir;
		vec3 VT = V - VN;
		m_pos = org + 0.005f * dir;
		m_oldPos = m_pos - (VT - VN * m_bounce);
	}
	}*/
}