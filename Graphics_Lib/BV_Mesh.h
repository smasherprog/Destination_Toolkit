#ifndef BV_MESH_H
#define BV_MESH_H

#include "Static_Mesh.h"

class BV_Mesh : public Static_Mesh{
public:

	BV_Mesh(){ }
	virtual ~BV_Mesh(){ DeInit();}

	virtual bool Init();
	virtual void Draw_BV(const mat4& view, const mat4& proj) override{}// this would be an infinite loop... not allowed!!
	virtual void Draw(const mat4& view, const mat4& proj);

};


#endif

