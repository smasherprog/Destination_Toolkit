#include "stdafx.h"
#include "cAccounts.h"
#include <fstream>
#include <iostream>

cInfo* cAccounts::find(std::string& key){ 
	std::map<std::string, cInfo*>::iterator beg(Accounts.find(key)); 
	if(beg == Accounts.end()) return NULL;
	return beg->second;
}
bool cAccounts::erase(std::string& key){ 
	std::map<std::string, cInfo*>::iterator beg(Accounts.find(key)); 
	if(beg == Accounts.end()) return false;
	delete beg->second;
	Accounts.erase(beg); 
	return true;
}
cInfo* cAccounts::insert(std::string& key){
	std::map<std::string, cInfo*>::iterator beg(Accounts.find(key)); 
	if(beg == Accounts.end()) return NULL;
	return new cInfo;
}


void cAccounts::Save(){
	std::cout<<std::endl<<"Shutting Down.. Performing Backups and Saving to Disk"<<std::endl;
	std::ofstream file(AccountsFile.c_str(), std::ios::out | std::ios::trunc);	
	if(!file){
		std::cout<<"Cannot Open File "<<AccountsFile<<" To save account infomation. You are soo scewed!"<<std::endl;
	} else {
		file<<Accounts.size();
		size_t size(0);
		for(std::map<std::string, cInfo*>::iterator beg(Accounts.begin()); beg!= Accounts.end(); beg++){
			beg->second->write(file);
			delete beg->second;
			size++;
		}
		file.close();
		std::cout<<"Successful Save of all accounts to "<<AccountsFile<<". Total Accounts saved:"<<size<<std::endl;	
	}
}

void cAccounts::Load(){
	std::ifstream file(AccountsFile.c_str());
	if(file){
		std::cout<<"Attempting to load Accounts from the file: "<<AccountsFile<<std::endl;
		LoadFile(file);
	} else {
		std::cout<<"Cannot Open "<<AccountsFile<<std::endl<<"There are no accounts loaded, Great Job!"<<std::endl;
		return;
	}
	std::cout<<"Accounts Loaded from file: "<<Accounts.size()<<std::endl;
}
void cAccounts::LoadFile(std::ifstream& file){
	size_t i(0);
	file>>i;
	std::cout<<"Accounts to Load: "<<i<<std::endl;
	
	for(size_t k(0); k<i; k++){
		if(!file){
			std::cout<<"Hit EOF before the loading was done. Error, some accounts may not be loaded correctly"<<std::endl;
			break;
		}
		cInfo* temp= new cInfo();
		temp->read(file);
		if(file.bad()){
			std::cout<<"I/O Error, some accounts may not be loaded correctly"<<std::endl;
			delete temp;
			break;
		}
		
		std::pair<std::map<std::string, cInfo*>::iterator, bool> be= Accounts.insert(std::map<std::string, cInfo*>::value_type(temp->Login, temp));
		if(be.second){
			std::cout<<"Inserted account with the login: "<<temp->Login<<std::endl;
			be.first->second=temp;
		} else delete temp;
	}
	file.close();
	file.clear();
}