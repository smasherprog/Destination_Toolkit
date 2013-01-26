#ifndef CWOODYLOADER_H
#define CWOODYLOADER_H
#include <string>
class cWoodyTree;

namespace cWoodyLoader{

#if defined(USE_WOODY3D)
bool Load(const std::string& xfile, cWoodyTree* mesh);
#endif



};



#endif