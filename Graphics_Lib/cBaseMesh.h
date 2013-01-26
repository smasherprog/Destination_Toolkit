#ifndef CMESH_H
#define CMESH_H
#include <vector>
#include "../Utilities/My_Timer.h"
#include "MeshTypes.h"
#include "../Utilities/MyMath.h"

#define OBJECTMOVEMENTDELTA 3.0f
/*
class cBaseMesh{
public:

	std::vector<vec3> Vertices;// this is used to calculate intersections
	Mesh_Lod Lod1, Lod2, Lod3;
	cAABB ObjectSpace_BV, WorldSpace_BV;// the extra bvs are to save on calculations
	std::string Name, FileName;
	mat4 Translation, Scaling, Rotation, World;
	vec3 MovementDelta;	
	quat Rot;
	euler Angles;
	bool Visible;
	My_Timer::tick_count LastQueryTime;
	Graphics::Query Query;
	bool InstanceChanged;

	cBaseMesh();
	virtual ~cBaseMesh();
	virtual void clear();
	virtual void clear(Mesh_Lod& lod);

	void InitAABB();
	// this must be called after a position change, scaling change, or rotation change. This is because this is an expensive operation, so it should be called explicitly
	virtual void UpdateWorldMatrix(){ World = Scaling * Rotation * Translation; WorldSpace_BV = ObjectSpace_BV; WorldSpace_BV.Transform(World); }

	virtual void DrawLod1(void* param1)=0;
	virtual void DrawLod2(void* param1)=0;
	virtual void DrawLod3(void* param1)=0;

	virtual void DrawShadowLod1(void* param1)=0;
	virtual void DrawShadowLod2(void* param1)=0;
	virtual void DrawShadowLod3(void* param1)=0;

	virtual float RayIntersect(const vec3& rayorig, const vec3& raydir)=0;
	virtual uint32_t GetType()=0;

	virtual void SetVisibility(bool v){ LastQueryTime=My_Timer::tick_count::now(); }

	virtual const std::string& GetFileName(){ return FileName; }
	virtual const std::string& GetName(){ return Name; }
	virtual void SetFileName(const std::string& fname){FileName =fname; }
	virtual void SetName(const std::string& name){ Name=name; }
	virtual bool HasBones(){ return false; }

	virtual void Save(ofstream& file);
	virtual void Save(const std::string& file)=0;// All Mesh should be loaded through this function
	virtual bool Load(ifstream& file);// All Mesh should be loaded through this function
	virtual bool Load(const std::string& file)=0;// All Mesh should be loaded through this function

	virtual void GetScaling(float& x, float &y, float& z){ x= Scaling._11; y = Scaling._22; z = Scaling._33;  }
	virtual void GetScalingX(float& x) { x= Scaling._11; }
	virtual void GetScalingY(float& x) { x= Scaling._22; }
	virtual void GetScalingZ(float& x) { x= Scaling._33; }
	virtual void SetScaling(const float& x, const float &y, const float& z) {Scaling._11=x; Scaling._22=y; Scaling._33=z; }
	virtual void SetScalingX(const float& x) { Scaling._11=x; }
	virtual void SetScalingY(const float& x) { Scaling._22=x; }
	virtual void SetScalingZ(const float& x) { Scaling._33=x; }

	//rotation should be in eular format. Positive rotation rotates to the right
	//heading is the amount of rotation about the y-axis
	//pitch is the amount of rotation about the x axis
	//bank is the amount of rotation about the z axis
	//GetRotation returns the angles as RADIANS>>> NOT DEGREEESS!!!
	virtual void GetRotation(quat& q){ q = Rot; }
	virtual void GetRotation(float& heading, float &pitch, float& bank){ heading=Angles.heading; pitch=Angles.pitch; bank=Angles.bank; }
	virtual void GetRotationX(float& x){ x=Angles.heading; }
	virtual void GetRotationY(float& x){ x=Angles.pitch; }
	virtual void GetRotationZ(float& x){ x=Angles.bank; }
	//SetRotation Expected the angles to be in RADIANS!!!!!!!
	virtual void SetRotation(const quat& q){
		Rot=q;
		Rotation.setupfromquat(Rot);
		Angles.fromObjectToWorldMatrix(Rotation);
	}
	virtual void SetRotation(const float& heading, const float &pitch, const float& bank){ Rot.SetupRotation(heading, pitch, bank); Rotation.setupfromquat(Rot); }
	virtual void SetRotationX(const float& x){ Angles.heading=x; Rot.SetupRotation(Angles); Rotation.setupfromquat(Rot); }
	virtual void SetRotationY(const float& x){ Angles.pitch=x; Rot.SetupRotation(Angles); Rotation.setupfromquat(Rot); }
	virtual void SetRotationZ(const float& x){ Angles.bank=x; Rot.SetupRotation(Angles); Rotation.setupfromquat(Rot); }

	virtual void GetPosition(float& x, float &y, float& z){ x=Translation._41; y =Translation._42; z = Translation._43;  }
	virtual void GetPositionX(float& x) { x= Translation._41; }
	virtual void GetPositionY(float& x) { x= Translation._42; }
	virtual void GetPositionZ(float& x) { x= Translation._43; }

	virtual void SetPosition(const float& x, const float &y, const float& z){ Translation._41=x; Translation._42=y; Translation._43=z;  }
	virtual void SetPositionX(const float& x) { Translation._41=x; }
	virtual void SetPositionY(const float& x) { Translation._42=x; }
	virtual void SetPositionZ(const float& x) { Translation._43=x; }
	
};
*/
#endif