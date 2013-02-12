#ifndef MESH_CONTAINER_H
#define MESH_CONTAINER_H
#include <string>
#include <vector>

class Base_Mesh;
class vec3;
class Base_Camera;
class Trans_Mesh;


class Mesh_Container_Service{
public:

	Mesh_Container_Service();
	virtual ~Mesh_Container_Service();

	bool Load_Mesh(std::string file);
	Base_Mesh* Check_Hit(vec3& ray, vec3& origin);


	std::vector<Base_Mesh*> Mesh;
	Trans_Mesh *Trans;
	Base_Mesh *BV;

};



#endif