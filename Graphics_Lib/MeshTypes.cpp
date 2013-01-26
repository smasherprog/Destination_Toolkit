#include "stdafx.h"
#include "MeshTypes.h"
#include "cBaseMesh.h"
#include "../Utilities/cSetArray.h"

template<class T>void ComputeNormals(vec3* Vertices, size_t numverts, T* indices, size_t numindices, vec3* normals){//used for 32 bit indicies
	size_t i = 0;
	memset(normals, 0, sizeof(vec3)* numverts);
	do{
		vec3 v0(Vertices[indices[i    ]]);
		vec3 v1(Vertices[indices[i + 1]]);
		vec3 v2(Vertices[indices[i + 2]]);
		vec3 normal(Cross(v1 - v0, v2 - v0));
		normal.normalize();
		normals[indices[i    ]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
		i += 3;
	} while(i < numindices);
	i=0;
	do{ normals[i].normalize(); } while(++i < numverts);
}

template<class T>void ComputeTangetBinormalNormal(vec3* Vertices, size_t numverts, T* indices, size_t numindices, vec3* normals, vec2* tex, vec3* tangents, vec3* binormals){
	size_t i =0;
	memset(normals, 0, sizeof(vec3)* numverts);
	memset(binormals, 0, sizeof(vec3)* numverts);
	memset(tangents, 0, sizeof(vec3)* numverts);
	do{
		vec3 v0(Vertices[indices[i    ]]);
		vec3 v1(Vertices[indices[i + 1]]);
		vec3 v2(Vertices[indices[i + 2]]);
		vec2 t0(tex[indices[i    ]]);
		vec2 t1(tex[indices[i + 1]]);
		vec2 t2(tex[indices[i + 2]]);
		vec3 sdir, tdir, normal;
		TangentVectors(v0, v1, v2, t0, t1, t2, sdir, tdir, normal);
		tangents[indices[i    ]] += sdir;
		tangents[indices[i + 1]] += sdir;
		tangents[indices[i + 2]] += sdir;
		binormals[indices[i    ]] += tdir;
		binormals[indices[i + 1]] += tdir;
		binormals[indices[i + 2]] += tdir;
		normals[indices[i    ]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
		i+=3;
	} while(i < numindices);
	i=0;
	do{		
		normals[i].normalize();
		const vec3& n =normals[i]; 
		const vec3& t = tangents[i];

		// Gram-Schmidt orthogonalize
		vec3 temp = t - n * Dot(n, t);
		temp.normalize();
		tangents[i] =temp;
		// Calculate handedness
		tangents[i]*= (Dot(Cross(n, t), binormals[i]) < 0.0F) ? -1.0F : 1.0F;
		tangents[i].normalize();
		binormals[i].normalize();
	} while( ++i < numverts);
}

template<class T>void ComputeNormalTangent(const vec3* Vertices, size_t numverts, T* indices, size_t numindices, vec3* normals, vec2* tex, vec3* tangents){
	size_t i =0;
	std::vector<vec3> binormals(numverts);	
	memset(normals, 0, sizeof(vec3)* numverts);
	memset(&binormals[0], 0, sizeof(vec3)* numverts);
	memset(tangents, 0, sizeof(vec3)* numverts);
	do{
		vec3 v0(Vertices[indices[i    ]]);
		vec3 v1(Vertices[indices[i + 1]]);
		vec3 v2(Vertices[indices[i + 2]]);
		vec2 t0(tex[indices[i    ]]);
		vec2 t1(tex[indices[i + 1]]);
		vec2 t2(tex[indices[i + 2]]);
		vec3 sdir, normal, tdir;
		TangentVectors(v0, v1, v2, t0, t1, t2, sdir, tdir, normal);
		tangents[indices[i    ]] += sdir;
		tangents[indices[i + 1]] += sdir;
		tangents[indices[i + 2]] += sdir;
		binormals[indices[i    ]] += tdir;
		binormals[indices[i + 1]] += tdir;
		binormals[indices[i + 2]] += tdir;
		normals[indices[i    ]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
		i+=3;
	} while(i < numindices);
	i=0;
	do{
		normals[i].normalize();
		const vec3& n =normals[i]; 
		const vec3& t = tangents[i];

		// Gram-Schmidt orthogonalize
		vec3 temp = t - n * Dot(n, t);
		temp.normalize();
		tangents[i] =temp;
		// Calculate handedness
		tangents[i] *= (Dot(Cross(n, t), binormals[i]) < 0.0F) ? -1.0F : 1.0F;

	} while( ++i < numverts);
}

template<class T>void ComputeTangents(vec3* Vertices, size_t numverts, T* indices, size_t numindices, vec3* normals, vec2* tex, vec3* tangents){
	size_t i =0;
	std::vector<vec3> binormals(numverts);

	memset(&binormals[0], 0, sizeof(vec3)* numverts);
	memset(tangents, 0, sizeof(vec3)* numverts);
	vec3 normtemp;
	do{
		vec3 v0(Vertices[indices[i    ]]);
		vec3 v1(Vertices[indices[i + 1]]);
		vec3 v2(Vertices[indices[i + 2]]);
		vec2 t0(tex[indices[i    ]]);
		vec2 t1(tex[indices[i + 1]]);
		vec2 t2(tex[indices[i + 2]]);
		vec3 sdir, tdir;
		TangentVectors(v0, v1, v2, t0, t1, t2, sdir, tdir, normtemp);
		tangents[indices[i    ]] += sdir;
		tangents[indices[i + 1]] += sdir;
		tangents[indices[i + 2]] += sdir;
		binormals[indices[i    ]] += tdir;
		binormals[indices[i + 1]] += tdir;
		binormals[indices[i + 2]] += tdir;
		i+=3;
	} while(i < numindices);
	i=0;
	do{ 
		const vec3& n = normals[i];
		const vec3& t = tangents[i];

		// Gram-Schmidt orthogonalize
		vec3 temp = t - n * Dot(n, t);
		temp.normalize();
		tangents[i] =temp;
		// Calculate handedness
		tangents[i] *= (Dot(Cross(n, t), binormals[i]) < 0.0F) ? -1.0F : 1.0F;

	} while( ++i < numverts);
}
void TangentVectors(const vec3 &v0, const vec3 &v1, const vec3 &v2, const vec2 &t0, const vec2 &t1, const vec2 &t2, vec3 &sdir){
	vec3 dv0(v1 - v0);
	vec3 dv1(v2 - v0);
	vec2 dt0(t1 - t0);
	vec2 dt1(t2 - t0);
	float r = 1.0f / (dt0.x * dt1.y - dt1.x * dt0.y);
	sdir = vec3(dt1.y * dv0.x - dt0.y * dv1.x, dt1.y * dv0.y - dt0.y * dv1.y, dt1.y * dv0.z - dt0.y * dv1.z) * r;

}
void TangentVectors(const vec3 &v1, const vec3 &v2, const vec3 &v3, const vec2 &w1, const vec2 &w2, const vec2 &w3, vec3 &sdir, vec3 &normal){
	float x1 = v2.x - v1.x;
	float x2 = v3.x - v1.x;
	float y1 = v2.y - v1.y;
	float y2 = v3.y - v1.y;
	float z1 = v2.z - v1.z;
	float z2 = v3.z - v1.z;

	float s1 = w2.x - w1.x;
	float s2 = w3.x - w1.x;
	float t1 = w2.y - w1.y;
	float t2 = w3.y - w1.y;

	float r = 1.0F / (s1 * t2 - s2 * t1);

	sdir = vec3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

	vec3 dv0(x1, y1, z1);
	vec3 dv1(x2, y2, z2);
	normal= Cross(dv0, dv1);
}
void TangentVectors(const vec3 &v1, const vec3 &v2, const vec3 &v3, const vec2 &w1, const vec2 &w2, const vec2 &w3, vec3 &sdir, vec3 &tdir, vec3 &normal){

	float x1 = v2.x - v1.x;
	float x2 = v3.x - v1.x;
	float y1 = v2.y - v1.y;
	float y2 = v3.y - v1.y;
	float z1 = v2.z - v1.z;
	float z2 = v3.z - v1.z;

	float s1 = w2.x - w1.x;
	float s2 = w3.x - w1.x;
	float t1 = w2.y - w1.y;
	float t2 = w3.y - w1.y;

	float r = 1.0F / (s1 * t2 - s2 * t1);

	sdir = vec3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
	tdir = vec3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

	vec3 dv0(x1, y1, z1);
	vec3 dv1(x2, y2, z2);
	normal= Cross(dv0, dv1);
}

float RayTriangleIntersect(const vec3& rayorig, const  vec3& ray, const  vec3* verts, const uint16_t* indices, const  size_t numindices){
	size_t i(0);
	do{
		float t(RayTriangleIntersect(rayorig, ray, verts[indices[i   ]], verts[indices[i  + 1]], verts[indices[i  + 2]]) );
		if( t!= INFINITY) return t;
		i+=3;
	} while(i < numindices);
	return INFINITY;
}
float RayTriangleIntersect( const vec3& rayorig, const vec3& ray, const vec3* verts, const  uint32_t* indices, const size_t numindices){
	size_t i(0);
	do{
		float t(RayTriangleIntersect(rayorig, ray, verts[indices[i   ]], verts[indices[i  + 1]], verts[indices[i  + 2]]) );
		if( t!= INFINITY) return t;
		i+=3;
	} while(i < numindices);
	return INFINITY;
}
float RayTriangleIntersect(const vec3& rayorig, const vec3& ray, const vec3& p0, const vec3& p1, const vec3& p2){// the ray can be any length  
	vec3 qvec;
	vec3 edge1(p1-p0);/* find std::vectors for two edges sharing vert0 */
	vec3 edge2(p2-p0);
	vec3 pvec(Cross(ray, edge2));/* begin calculating determinant - also used to calculate U parameter */
	float det = Dot(edge1, pvec);/* if determinant is near zero, ray lies in plane of triangle */
	if (det > MATH_EPS) {
		vec3 tvec(rayorig- p0);/* calculate Lengthance from vert0 to ray origin */
		float u(Dot(tvec, pvec));
		if ((u < 0.0f) | (u > det)) return INFINITY;/* calculate U parameter and test bounds */
		qvec=Cross(tvec, edge1);/* prepare to test V parameter */
		float v(Dot(ray, qvec));/* calculate V parameter and test bounds */
		if ((v < 0.0f) | (u + v > det)) return INFINITY;
	} else if(det < -MATH_EPS) {
		vec3 tvec(rayorig-p0);/* calculate Lengthance from vert0 to ray origin */
		float u(Dot(tvec, pvec));/* calculate U parameter and test bounds */
		if ((u > 0.0f) | (u < det)) return INFINITY;
		qvec=Cross(tvec, edge1);/* prepare to test V parameter */
		float v(Dot(ray, qvec)) ;/* calculate V parameter and test bounds */
		if ((v > 0.0f) | (u + v < det)) return INFINITY;
	} else return INFINITY; /* ray is parallell to the plane of the triangle */
	return Dot(edge2, qvec) / det;/* calculate t, ray intersects triangle */
}
void Subdivide(std::vector<vec4>& vertices, std::vector<uint16_t>& indices){
	std::vector<vec4> vin = vertices;
	std::vector<uint16_t>  iin = indices;
	vertices.resize(0);
	indices.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	UINT numTris = (UINT)iin.size()/3;
	for(UINT i = 0; i < numTris; ++i){
		vec4 v0 = vin[ iin[i*3+0] ];
		vec4 v1 = vin[ iin[i*3+1] ];
		vec4 v2 = vin[ iin[i*3+2] ];
		vec4 m0, m1,m2;
		m0 = (v0 + v1)*.5f;
		m1 = (v1 + v2)*.5f;
		m2 = (v0 + v2)*.5f;
		vertices.push_back(v0); // 0
		vertices.push_back(v1); // 1
		vertices.push_back(v2); // 2
		vertices.push_back(m0); // 3
		vertices.push_back(m1); // 4
		vertices.push_back(m2); // 5
		indices.push_back(i*6+0);
		indices.push_back(i*6+3);
		indices.push_back(i*6+5);
		indices.push_back(i*6+3);
		indices.push_back(i*6+4);
		indices.push_back(i*6+5);
		indices.push_back(i*6+5);
		indices.push_back(i*6+4);
		indices.push_back(i*6+2);
		indices.push_back(i*6+3);
		indices.push_back(i*6+1);
		indices.push_back(i*6+4);
	}
}
void UtilizePosNormTex(void* sourceVerts, vec3* pos, vec3* nor, vec2* tex, vec3* tan, mat4* trasnform, size_t dwNumVerts, uint16_t* indices, size_t stride, size_t numindices){
	struct pnt{ vec3 pos, norm; vec2 tex; };
	pnt* data = reinterpret_cast<pnt*>(sourceVerts);
	for(size_t i(0); i < dwNumVerts; i++) {
		pos[i]= data[i].pos;
		nor[i]= data[i].norm;
		tex[i]= data[i].tex;
	}
	if(tan==NULL) return;
	if(stride ==4) ComputeTangents(pos, dwNumVerts, reinterpret_cast<uint32_t*>(indices), numindices, nor, tex, tan);
	else ComputeTangents(pos, dwNumVerts, indices, numindices, nor, tex, tan);
	if(trasnform==NULL) return;
	for(size_t i(0); i < dwNumVerts; i++) {
		pos[i]*=*trasnform;
		TransformNormal(nor[i], *trasnform);
		TransformNormal(tan[i], *trasnform);
	}
}

void UtilizePosTexNorm(void* sourceVerts, vec3* pos, vec3* nor, vec2* tex, vec3* tan, mat4* trasnform, size_t dwNumVerts, uint16_t* indices, size_t stride, size_t numindices){
	struct pnt{ vec3 pos; vec2 tex; vec3 norm; };
	pnt* data = reinterpret_cast<pnt*>(sourceVerts);
	for(size_t i(0); i < dwNumVerts; i++){
		pos[i]= data[i].pos;
		nor[i]= data[i].norm;
		tex[i]= data[i].tex;
	}
	if(tan==NULL) return;
	if(stride ==4) ComputeTangents(pos, dwNumVerts, reinterpret_cast<uint32_t*>(indices), numindices, nor, tex, tan);
	else ComputeTangents(pos, dwNumVerts, indices, numindices, nor, tex, tan);
	if(trasnform==NULL) return;
	for(size_t i(0); i < dwNumVerts; i++){
		pos[i]*=*trasnform;
		TransformNormal(nor[i], *trasnform);
		TransformNormal(tan[i], *trasnform);
	}
}
void UtilizePosTex(void* sourceVerts, vec3* pos, vec3* nor, vec2* tex,  vec3* tan, mat4* trasnform, size_t dwNumVerts, uint16_t* indices, size_t stride, size_t numindices){
	struct pnt{ vec3 pos; vec2 tex;};
	pnt* data = reinterpret_cast<pnt*>(sourceVerts);
	for(size_t i(0); i < dwNumVerts; i++){
		pos[i]= data[i].pos;
		tex[i]= data[i].tex;
	}
	if(tan==NULL){
		if(nor ==NULL) return;
		if(stride ==4) ComputeNormals(pos, dwNumVerts, reinterpret_cast<uint32_t*>(indices), numindices, nor);
		else ComputeNormals(pos, dwNumVerts, indices, numindices, nor);
		return;
	}
	if(stride ==4) ComputeNormalTangent(pos, dwNumVerts, reinterpret_cast<uint32_t*>(indices), numindices, nor, tex, tan);
	else ComputeNormalTangent(pos, dwNumVerts, indices, numindices, nor, tex, tan);
	if(trasnform==NULL) return;
	for(size_t i(0); i < dwNumVerts; i++){
		pos[i]*=*trasnform;
		TransformNormal(nor[i], *trasnform);
		TransformNormal(tan[i], *trasnform);
	}
}

void GenGrid(std::vector<uint16_t>& indices, std::vector<vec3>& vertices, uint32_t size){
	indices.resize(6*(size-1)*(size-1)); 
	vertices.resize(size*size);
	float invsize = 1.0f/(size-1);// the grid goes from size-1 to 0
	for(float i(0); i < size; i+=1.0f){// each grid will go from (0, 0) to (1, 1)
		for(float j(0); j < size; j+=1.0f){// each grid will go from (0, 0) to (1, 1)
			vertices[ (unsigned int)i*size + (unsigned int)j] = vec3(i*invsize, j*invsize, 0.0f);
		}
	}

	unsigned int off =0;
	for(uint32_t i(0); i< size-1; i++){
		for(uint32_t j(0); j< size-1; j++){
			indices[off    ] = i*size + j;
			indices[off + 1] = i*size + j + 1;
			indices[off + 2] = (i+1)*size + j;
			indices[off + 3] = (i+1)*size + j;
			indices[off + 4] = i*size + j + 1;
			indices[off + 5] = (i+1)*size + j + 1;
			off+=6;
		}
	}
}

void GenSphere(float mRadius, uint32_t mNumSlices, uint32_t mNumStacks, std::vector<vec3>& vertices, std::vector<Vertex_Types::Tex_Norm>& texnorm, std::vector<uint16_t>& indices){
	float phiStep = Pi/mNumStacks;
	// do not count the poles as rings
	uint32_t numRings = mNumStacks-1;
	// Compute vertices for each stack ring.
	for(uint32_t i = 1; i <= numRings; ++i) {
		float phi = i*phiStep;
		// vertices of ring
		float thetaStep = 2.0f*Pi/mNumSlices;
		for(uint32_t j = 0; j <= mNumSlices; ++j) {
			float theta = j*thetaStep;// spherical to cartesian	
			vec3 v( mRadius*sinf(phi)*cosf(theta), mRadius*cosf(phi), mRadius*sinf(phi)*sinf(theta) );
			vertices.push_back( v );
			Vertex_Types::Tex_Norm tn;
			v.normalize();
			tn.Norm = v;
			tn.Tex = vec2(theta / (2.0f*Pi), phi / Pi);
			texnorm.push_back(tn);
		}
	}
	// poles: note that there will be texture coordinate distortion
	vertices.push_back( vec3(0.0f, -mRadius, 0.0f) );
	vertices.push_back( vec3(0.0f, mRadius, 0.0f) );
	uint32_t northPoleIndex = (uint32_t)vertices.size()-1;
	uint32_t southPoleIndex = (uint32_t)vertices.size()-2;
	uint32_t numRingVertices = mNumSlices+1;
	// Compute indices for inner stacks (not connected to poles).
	for(uint32_t i = 0; i < mNumStacks-2; ++i){
		for(uint32_t j = 0; j < mNumSlices; ++j){
			indices.push_back(i*numRingVertices + j);
			indices.push_back(i*numRingVertices + j+1);
			indices.push_back((i+1)*numRingVertices + j);
			indices.push_back((i+1)*numRingVertices + j);
			indices.push_back(i*numRingVertices + j+1);
			indices.push_back((i+1)*numRingVertices + j+1);
		}
	}
	// Compute indices for top stack.  The top stack was written 
	// first to the vertex buffer.
	for(uint32_t i = 0; i < mNumSlices; ++i){
		indices.push_back(northPoleIndex);
		indices.push_back(i+1);
		indices.push_back(i);
	}
	// Compute indices for bottom stack.  The bottom stack was written
	// last to the vertex buffer, so we need to offset to the index
	// of first vertex in the last ring.
	uint32_t baseIndex = (numRings-1)*numRingVertices;
	for(uint32_t i = 0; i < mNumSlices; ++i){
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex+i);
		indices.push_back(baseIndex+i+1);
	}
}
void GenCube(std::vector<vec3>& vertices, std::vector<uint16_t>& indices){
	indices.resize(36);
	vertices.resize(8);
	vec3 min_=vec3(-1.0f, -1.0f, -1.0f);
	vec3 max_=vec3(1.0f, 1.0f, 1.0f);
	vertices[0] = min_;
	vertices[1] = vec3(min_.x, max_.y, min_.z); 
	vertices[2] = vec3(max_.x, max_.y, min_.z);
	vertices[3] = vec3(max_.x, min_.y, min_.z);
	vertices[4] = vec3(min_.x, min_.y, max_.z);
	vertices[5] = vec3(min_.x, max_.y, max_.z);
	vertices[6] = max_;
	vertices[7] = vec3(max_.x, min_.y, max_.z);

	const uint16_t ind[] = {
		// front face
		0, 1, 2,
		0, 2, 3,
		// back face
		4, 6, 5,
		4, 7, 6,
		// left face
		4, 5, 1,
		4, 1, 0,
		// right face
		3, 2, 6,
		3, 6, 7,
		// top face
		1, 5, 6,
		1, 6, 2,
		// bottom face
		4, 0, 3, 
		4, 3, 7
	};
	for(size_t i(0); i< 36; i++){// the compiler will optimize this away since it is known at compile time
		indices[i]=ind[i];
	}
}
void GenCylinder( float mTopRadius, float mBottomRadius, float mHeight, uint32_t mNumSlices, uint32_t mNumStacks, std::vector<vec3>& vertices, std::vector<Vertex_Types::Tex_Norm>& texnorm, std::vector<uint16_t>& indices){
	float stackHeight = mHeight / mNumStacks;
	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (mTopRadius - mBottomRadius) / mNumStacks;
	uint32_t numRings = mNumStacks+1;
	// Compute vertices for each stack ring.
	for(uint32_t i = 0; i < numRings; ++i){
		float y = -0.5f*mHeight + i*stackHeight;
		float r = mBottomRadius + i*radiusStep;

		// Height and radius of next ring up.
		float y_next = -0.5f*mHeight + (i+1)*stackHeight;
		float r_next = mBottomRadius + (i+1)*radiusStep;

		// vertices of ring
		float dTheta = 2.0f*Pi/mNumSlices;
		for(uint32_t j = 0; j <= mNumSlices; ++j){
			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);
			float u = (float)j/mNumSlices;
			float v = 1.0f - (float)i/mNumStacks;
			// Partial derivative in theta direction to get tangent vector (this is a unit vector).
			vec3 T(-s, 0.0f, c);
			// Compute tangent vector down the slope of the cone (if the top/bottom 
			// radii differ then we get a cone and not a true cylinder).
			vec3 P(r*c, y, r*s);
			vec3 P_next(r_next*c, y_next, r_next*s);
			vec3 B = P - P_next;
			B.normalize();

			vec3 N = Cross(T, B);
			N.normalize();

			vertices.push_back( vec3(P.x, P.y, P.z));
			Vertex_Types::Tex_Norm tn;
			tn.Norm = vec3(N.x, N.y, N.z);
			tn.Tex = vec2(u, v);
			texnorm.push_back(tn);
		}
	}

	uint32_t numRingVertices = mNumSlices+1;

	// Compute indices for each stack.
	for(uint32_t i = 0; i < mNumStacks; ++i){
		for(uint32_t j = 0; j < mNumSlices; ++j){
			indices.push_back(i*numRingVertices + j);
			indices.push_back((i+1)*numRingVertices + j);
			indices.push_back((i+1)*numRingVertices + j+1);

			indices.push_back(i*numRingVertices + j);
			indices.push_back((i+1)*numRingVertices + j+1);
			indices.push_back(i*numRingVertices + j+1);
		}
	}
	uint32_t baseIndex = (uint32_t)vertices.size();
	// Duplicate cap vertices because the texture coordinates and normals differ.
	float y = 0.5f*mHeight;
	// vertices of ring
	float dTheta = 2.0f*Pi/mNumSlices;
	for(uint32_t i = 0; i <= mNumSlices; ++i){
		float x = mTopRadius*cosf(i*dTheta);
		float z = mTopRadius*sinf(i*dTheta);

		// Map [-1,1]-->[0,1] for planar texture coordinates.
		float u = +0.5f*x/mTopRadius + 0.5f;
		float v = -0.5f*z/mTopRadius + 0.5f;
		vertices.push_back( vec3(x, y, z));
		Vertex_Types::Tex_Norm tn;
		tn.Norm = vec3(0.0f, 1.0f, 0.0f);
		tn.Tex = vec2(u, v);
		texnorm.push_back(tn);
	}

	vertices.push_back( vec3(0.0f, y, 0.0f));
	Vertex_Types::Tex_Norm tn;
	tn.Norm = vec3(0.0f, 1.0f, 0.0f);
	tn.Tex = vec2(0.5f, 0.5f);
	texnorm.push_back(tn);

	// index of center vertex
	uint32_t centerIndex = (uint32_t)vertices.size()-1;

	for(uint32_t i = 0; i < mNumSlices; ++i){
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i+1);
		indices.push_back(baseIndex + i);
	}

	baseIndex = (uint32_t)vertices.size();

	// Duplicate cap vertices because the texture coordinates and normals differ.
	y = -0.5f*mHeight;

	// vertices of ring
	dTheta = 2.0f*Pi/mNumSlices;
	for(uint32_t i = 0; i <= mNumSlices; ++i){
		float x = mBottomRadius*cosf(i*dTheta);
		float z = mBottomRadius*sinf(i*dTheta);

		// Map [-1,1]-->[0,1] for planar texture coordinates.
		float u = +0.5f*x/mBottomRadius + 0.5f;
		float v = -0.5f*z/mBottomRadius + 0.5f;
		vertices.push_back( vec3(x, y, z));
		Vertex_Types::Tex_Norm tn;
		tn.Norm = vec3(0.0f, -1.0f, 0.0f);
		tn.Tex = vec2(u, v);
		texnorm.push_back(tn);
	}
	vertices.push_back( vec3(0.0f, y, 0.0f));
	tn.Norm = vec3(0.0f, -1.0f, 0.0f);
	tn.Tex = vec2(0.5f, 0.5f);
	texnorm.push_back(tn);

	// index of center vertex
	centerIndex = (uint32_t)vertices.size()-1;

	for(uint32_t i = 0; i < mNumSlices; ++i){
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i+1);
	}
}
void Batch::Save(std::ofstream& stream){
	stream.write((char*)&NumIndices, sizeof(unsigned int));
	stream.write((char*)&StartIndex, sizeof(unsigned int));
	stream.write((char*)&NumVerts, sizeof(unsigned int));
	stream.write((char*)&_Material, sizeof(Material));

	unsigned int count =0;
	for(int i=0; i< MAX_TEXTUREUNIT; i++){
		if(Maps[i]->Empty()) continue;
		count+=1;
	}
	//write the count
	stream.write((char*)&count, sizeof(unsigned int));
	//now write the textures
	for(int i=0; i< MAX_TEXTUREUNIT; i++){
		if(Maps[i]->Empty()) continue;
		std::string name = Maps[i]->FileName;
		unsigned int temp = (unsigned int)name.size()+1;
		stream.write((char*)&temp, sizeof(unsigned int));
		stream.write(name.c_str(), temp);
		temp = (unsigned int)Maps[i]->Get_MapType();
		stream.write((char*)&temp, sizeof(unsigned int));
	}
}
void Batch::Load(std::ifstream& stream){
	stream.read((char*)&NumIndices, sizeof(unsigned int));
	stream.read((char*)&StartIndex, sizeof(unsigned int));
	stream.read((char*)&NumVerts, sizeof(unsigned int));
	stream.read((char*)&_Material, sizeof(Material));

	// diffuse tex
	std::vector<char> name;
	unsigned int temp =0;
	unsigned int count =0;
	stream.read((char*)&count, sizeof(unsigned int));
	for(unsigned int i=0; i< count; i++){
		stream.read((char*)&temp, sizeof(unsigned int));
		name.resize(temp);
		stream.read(&name[0], temp);
		stream.read((char*)&temp, sizeof(unsigned int));
		Graphics::Texture* t = GetMap((Map_Type)temp);
		if(t==NULL) continue;
		t->Create(std::string(&name[0], temp));

	}
}