// g++ -g3 -ggdb -O0 -DDEBUG -Wall -Wextra fhmqv-agree.cpp -o fhmqv-agree.exe -lcryptopp -lpthread
// g++ -g -O2 -DNDEBUG -Wall -Wextra fhmqv-agree.cpp -o fhmqv-agree.exe -lcryptopp -lpthread

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <stdexcept>
using std::runtime_error;

#include "cryptopp/osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include "cryptopp/fhmqv.h"
#include "cryptopp/eccrypto.h"
using CryptoPP::ECP;
using CryptoPP::FHMQV;

#include "cryptopp/secblock.h"
using CryptoPP::SecByteBlock;

// ASN1 is a namespace, not an object
#include "cryptopp/asn.h"
#include "cryptopp/oids.h"
using CryptoPP::OID;
using namespace CryptoPP::ASN1;

#include "cryptopp/integer.h"
using CryptoPP::Integer;

#include "cryptopp/sha.h"
using CryptoPP::SHA256;

//#include "cryptopp/hex.h"
//using CryptoPP::HexEncoder;
//
//#include "cryptopp/files.h"
//using CryptoPP::FileSink;

using namespace CryptoPP;
#include <assert.h>

int main(int argc, char* argv[])
{
  OID CURVE = secp256r1();
  AutoSeededRandomPool rng;

  // FHMQV< ECP, DL_GroupParameters_EC< ECP >::DefaultCofactorOption, SHA256 >::Domain fhmqv256( CURVE, true /*client*/ );
  FHMQV < ECP >::Domain fhmqvA( CURVE, true /*client*/ ), fhmqvB( CURVE, false /*server*/ );

  // Party A, static (long term) key pair
  SecByteBlock sprivA(fhmqvA.StaticPrivateKeyLength()), spubA(fhmqvA.StaticPublicKeyLength());
  // Party A, ephemeral (temporary) key pair
  SecByteBlock eprivA(fhmqvA.EphemeralPrivateKeyLength()), epubA(fhmqvA.EphemeralPublicKeyLength());

  // Party B, static (long term) key pair
  SecByteBlock sprivB(fhmqvB.StaticPrivateKeyLength()), spubB(fhmqvB.StaticPublicKeyLength());
  // Party B, ephemeral (temporary) key pair
  SecByteBlock eprivB(fhmqvB.EphemeralPrivateKeyLength()), epubB(fhmqvB.EphemeralPublicKeyLength());

  // Imitate a long term (static) key
  fhmqvA.GenerateStaticKeyPair(rng, sprivA, spubA);
  // Ephemeral (temporary) key
  fhmqvA.GenerateEphemeralKeyPair(rng, eprivA, epubA);

  // Imitate a long term (static) key
  fhmqvB.GenerateStaticKeyPair(rng, sprivB, spubB);
  // Ephemeral (temporary) key
  fhmqvB.GenerateEphemeralKeyPair(rng, eprivB, epubB);

  if(fhmqvA.AgreedValueLength() != fhmqvB.AgreedValueLength())
    throw runtime_error("Shared secret size mismatch");

  SecByteBlock sharedA(fhmqvA.AgreedValueLength()), sharedB(fhmqvB.AgreedValueLength());

  if(!fhmqvA.Agree(sharedA, sprivA, eprivA, spubB, epubB))
    throw runtime_error("Failed to reach shared secret (A)");

  if(!fhmqvB.Agree(sharedB, sprivB, eprivB, spubA, epubA))
    throw runtime_error("Failed to reach shared secret (B)");

  Integer ssa, ssb;
  ssa.Decode(sharedA.BytePtr(), sharedA.SizeInBytes());
  cout << "(A): " << std::hex << ssa << endl;
  ssb.Decode(sharedB.BytePtr(), sharedB.SizeInBytes());
  cout << "(B): " << std::hex << ssb << endl;

  if(ssa != ssb)
    throw runtime_error("Failed to reach shared secret (C)");

  cout << "Agreed to shared secret" << endl;

  //HexEncoder encoder(new FileSink("fhmqv256.dat", false /*binary*/));
  //fhmqvB.AccessGroupParameters().DEREncode(encoder);
  //encoder.MessageEnd();

  return 0;
}
