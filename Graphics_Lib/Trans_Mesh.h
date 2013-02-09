#ifndef TRANS_MESH_H
#define TRANS_MESH_H

#include "Static_Mesh.h"

class Trans_Mesh : public Static_Mesh{
public:

	Trans_Mesh(){ }
	virtual ~Trans_Mesh(){ DeInit();}

	virtual bool Init();

	virtual void Draw(const mat4& view, const mat4& proj);

};


#endif

