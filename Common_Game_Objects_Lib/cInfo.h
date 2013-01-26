#ifndef CINFO_H
#define CINFO_H

#include <string>


class cPeer;

class cInfo{
public:
	std::string Login, Password;
	unsigned char Type, Active;// account type gm user
	unsigned char ServerIndex;
	bool LoggedIn;
	unsigned int PacketsNotSize, CalledwoPermission;
	cPeer* Peer;

	cInfo(){ clear(); }

	void SetLogin(std::string& l){ Login =l; }
	void SetPassword(std::string& l){ Password =l; }
	const std::string& GetLogin(){ return Login; }
	const std::string& GetPassword(){ return Password; }

	void Out();
	void clear();
	void write(std::ofstream &stream) const;
	void read(std::ifstream &stream);
	
};
std::ostream& operator<<(std::ostream& out, const cInfo& obj);
#endif
