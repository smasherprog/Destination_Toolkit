#include "cInfo.h"
#include "..\Network_Lib\cPeer.h"
#include <fstream>
#include <iostream>
#include "cBaseNetwork.h"

std::ostream& operator<<(std::ostream& out, const cInfo& obj){
	out<<obj.Login<<"  ";
	out<<obj.Password<<"  ";
	out<<obj.PacketsNotSize<<"  ";
	out<<obj.CalledwoPermission<<"  ";
	out<<(unsigned short int)obj.Type<<"  ";
	out<<(unsigned short int)obj.Active<<"  "<<std::endl;
	out<<obj.ServerIndex<<"  ";
	out<<obj.LoggedIn<<"  ";
	return out;
}

void cInfo::clear(){
	Login.clear();
	Password.clear();
	LoggedIn=false;
	PacketsNotSize=CalledwoPermission=0;
	Type=Network::DUMMY_ACCOUNT;
	Active=Network::NOTACTIVE;
	ServerIndex=INVALID_SERVER_INDEX;
	Peer=0;
}
void cInfo::write(std::ofstream &stream) const{
	std::cout<<"Writing account to file"<<std::endl<<"\tLogin: "<<Login<<std::endl;
	stream<<std::endl<<Login<<std::endl;
	stream<<Password<<std::endl;
	std::cout<<"\tPassword: "<<Password<<std::endl;
	stream<<Type<<std::endl;
	std::cout<<"\tAccount Type ";
	if(Type==Network::USER_ACCOUNT){
		std::cout<<"User";
	} else if(Type==Network::GM_ACCOUNT){
		std::cout<<"GM";
	} else if(Type==Network::SERVER_ACCOUNT){
		std::cout<<"Server";
	} else if(Type==Network::ADMIN_ACCOUNT){
		std::cout<<"Administrator";
	} else {
		std::cout<<"Unrecognized account type";
	}
	stream<<Active<<std::endl;
	std::cout<<std::endl<<"\tAcstd::cout Status: ";
	if(Active==Network::ACTIVE){
		std::cout<<"Active";
	} else {
		std::cout<<"Not Active";
	}
	stream<<PacketsNotSize<<std::endl;
	std::cout<<std::endl<<"\tPackets Sent that were not correct size: "<<PacketsNotSize<<std::endl;
	stream<<CalledwoPermission;
	std::cout<<"\tFunctions called that the account did not have permission: "<<CalledwoPermission<<std::endl;
}
void cInfo::read(std::ifstream &stream){
	clear();
	stream>>Login;
	std::cout<<"Reading account from file"<<std::endl<<"\tLogin: "<<Login<<std::endl;
	stream>>Password;
	std::cout<<"\tPassword: "<<Password<<std::endl;
	stream>>Type;
	std::cout<<"\tAccount Type ";
	if(Type==Network::USER_ACCOUNT){
		std::cout<<"User";
	} else if(Type==Network::GM_ACCOUNT){
		std::cout<<"GM";
	} else if(Type==Network::SERVER_ACCOUNT){
		std::cout<<"Server";
	} else if(Type==Network::ADMIN_ACCOUNT){
		std::cout<<"Administrator";
	} else {
		std::cout<<"Unrecognized account type";
	}
	stream>>Active;
	std::cout<<std::endl<<"\tAcstd::cout Status: ";
	if(Active==Network::ACTIVE){
		std::cout<<"Active";
	} else {
		std::cout<<"Not Active";
	}
	stream>>PacketsNotSize;
	std::cout<<std::endl<<"\tPackets Sent that were not correct size: "<<PacketsNotSize<<std::endl;
	stream>>CalledwoPermission;
	std::cout<<"\tFunctions called that the account did not have permission: "<<CalledwoPermission<<std::endl;
}