#include "stdafx.h"
#include "NetworkUtils.h"

#ifdef _WIN32
unsigned int Network::GetTime(){ return timeGetTime(); }
#else
unsigned int Network::GetTime(){
	struct timeval timeVal;
	gettimeofday (& timeVal, NULL);
	return timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;
}
#endif
#if defined(USEENCRYPTION)
Network::cEncryptionInfo::cEncryptionInfo() : AESKey(SHA256::DIGESTSIZE), fhmqv(0) {}
Network::cEncryptionInfo::~cEncryptionInfo() {
	delete fhmqv;
}
void Network::cEncryptionInfo::clear(){// clear everything
	clear_keyexchange();
	memset(AESKey.BytePtr(), 0, AESKey.SizeInBytes());
	delete fhmqv;
	fhmqv=0;
}
void Network::cEncryptionInfo::clear_keyexchange(){// clear only the keys after the initial key  exchange is finished, keep the AES key intact, delete the ECC domain params
	memset(ephemeralprivatekey.BytePtr(), 0, ephemeralprivatekey.SizeInBytes());
	memset(staticprivatekey.BytePtr(), 0, staticprivatekey.SizeInBytes());
	memset(staticpublickey.BytePtr(), 0, staticpublickey.SizeInBytes());
	memset(ephemeralpublickey.BytePtr(), 0, ephemeralpublickey.SizeInBytes());
	delete fhmqv;
	fhmqv=0;
}
void Network::cEncryptionInfo::Init(bool client){
	clear();// just in case
	const OID CURVE = secp256r1();
	fhmqv = new FHMQV< ECP >::Domain(CURVE, client);// allocate here
	staticprivatekey.resize(fhmqv->StaticPrivateKeyLength());
	staticpublickey.resize(fhmqv->StaticPublicKeyLength());
	ephemeralprivatekey.resize(fhmqv->EphemeralPrivateKeyLength());
	ephemeralpublickey.resize(fhmqv->EphemeralPublicKeyLength());
}
#endif