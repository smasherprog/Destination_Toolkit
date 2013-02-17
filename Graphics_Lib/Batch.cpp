#include "stdafx.h"
#include "Batch.h"

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