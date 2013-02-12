#ifndef BASE_MESH_H
#define BASE_MESH_H

#include <string>
#include <vector>
#include "../Graphics_Lib/GraphicsCore.h"
#include "../Utilities/My_Timer.h"
#include "../BV_Lib/AABB.h"
#include "MeshTypes.h"
#include "../Utilities/Signal.h"

class mat4;
class vec3;
class Base_Camera;
class Base_Mesh{
public:

	Base_Mesh(){ DeInit();}
	virtual ~Base_Mesh();

	virtual bool Init()=0;
	virtual void DeInit();
	virtual bool Save(const std::string& file)=0;
	virtual bool Load(const std::string& file)=0;

	virtual void Draw(const Base_Camera* camera) =0;

	virtual mat4 GetWorld() const{ return Scaling * Rotation * Translation; }

	virtual void SetVisibility() const{ }

	virtual float Ray_Tri_Intersect(const vec3& rayorig, const vec3& raydir) =0;// more expensive test checks all triangles
	virtual float Ray_BV_Intersect(const vec3& rayorig, const vec3& raydir) =0;// just does a BV check, cheap but fast

	virtual vec3 GetScaling() const { return vec3(Scaling._11,Scaling._22,Scaling._33);  }
	virtual void SetScaling(const vec3& v) {Scaling._11=v.x; Scaling._22=v.y; Scaling._33=v.z; }

	virtual euler GetRotation() const { return euler(Angles); }
	virtual void SetRotation(const quat& q){
		Rot=q;
		Rotation.setupfromquat(Rot);
		Angles.fromObjectToWorldMatrix(Rotation);
	}
	virtual void SetRotation(const euler& e){ Rot.SetupRotation(e); Rotation.setupfromquat(Rot); }

	virtual vec3 GetPosition() const { return vec3(Translation._41,Translation._42,Translation._43);  }
	virtual void SetPosition(const vec3& newpos) { Translation._41=newpos.x; Translation._42=newpos.y; Translation._43=newpos.z; On_SetPosition.Call(newpos); }
	virtual void Translate_By_Offset(const vec3& offset){ Translation._41+=offset.x; Translation._42+=offset.y; Translation._43+=offset.z; On_Translate_By_Offset.Call(offset); }

	void GetScaling(mat4** m) { *m=&Scaling; }
	void GetRotation(mat4** m) { *m=&Rotation; }
	void GetRotation(quat** m) { *m=&Rot; }
	void GetTranslation(mat4** m) { *m=&Translation; }

	virtual float Get_Max_x_Size()const =0 ;
	virtual float Get_Max_y_Size()const =0 ;
	virtual float Get_Max_z_Size()const =0 ;
	virtual vec3 Get_Center()const =0 ;

	virtual std::string GetName() const{ return Name;}
	virtual void SetName(std::string& n) {Name = n;}

	MY_Utilities::Signal_st<void, vec3> On_SetPosition;
	MY_Utilities::Signal_st<void, vec3> On_Translate_By_Offset;

protected:

	void Load(std::ifstream& stream);
	void Save(std::ofstream& stream);

	std::string Name, FileName;
	mat4 Translation, Scaling, Rotation, World;
	quat Rot;
	euler Angles;

	bool Visible;
	vec3 MovementDelta;	
	My_Timer::tick_count LastQueryTime;
	Graphics::Query Query;

};



#endif