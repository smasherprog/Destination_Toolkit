#ifndef MESH_CONTAINER_H
#define MESH_CONTAINER_H
#include <string>
#include <vector>

class Base_Mesh;
class vec3;
class Base_Camera;
class Mesh_Container{
public:

	Mesh_Container(){}
	~Mesh_Container();

	bool Load_Mesh(std::string file);
	Base_Mesh* Check_Hit(vec3& ray, vec3& origin);
	void Draw(const Base_Camera* camera);


protected:
	std::vector<Base_Mesh*> Mesh;
};



#endif