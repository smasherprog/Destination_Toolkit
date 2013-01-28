#ifndef MESH_CONTAINER_H
#define MESH_CONTAINER_H
#include <string>
#include <vector>

class Base_Mesh;
class vec3;
class mat4;
class Mesh_Container{
public:

	Mesh_Container(){}
	~Mesh_Container();

	bool Load_Mesh(std::string file);
	Base_Mesh* Check_Hit(vec3& ray, vec3& origin);
	void Draw(mat4& view, mat4& proj);


protected:
	std::vector<Base_Mesh*> Mesh;
};



#endif