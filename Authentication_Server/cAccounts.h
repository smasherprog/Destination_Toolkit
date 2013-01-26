#ifndef CACCOUNTS_H
#define CACCOUNTS_H

#include <string>
#include "../Common_Game_Objects_Lib/cInfo.h"
#include <map>

class cAccounts{
public:

	cAccounts(): AccountsFile("../Assets/Accounts.ifo"), UpdateAccountsFile("../Assets/AccountUpdates.ifo"), AccountsBackUp("../Assets/AccountsBackUp.ifo"){ Load(); }
	~cAccounts(){ Save(); }

	size_t size(){ return Accounts.size(); }
	
	cInfo* insert(std::string& key);// for creating accounts
	cInfo* find(std::string& key); 
	bool erase(std::string& key);// for destroying accounts

private:
	
	const std::string AccountsFile, UpdateAccountsFile, AccountsBackUp;
	std::map<std::string, cInfo*> Accounts;

	void Save();
	void Load();
	void LoadFile(std::ifstream& file);


};
#endif