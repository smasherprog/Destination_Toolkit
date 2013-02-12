#ifndef BV_MESH_H
#define BV_MESH_H

#include "Static_Mesh.h"

class BV_Mesh : public Static_Mesh{
public:

	BV_Mesh(){ }
	virtual ~BV_Mesh(){ DeInit();}

	virtual bool Init();
	virtual void Draw(const Base_Camera* camera)override;

};


#endif

