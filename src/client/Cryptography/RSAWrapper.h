#pragma once

#include <osrng.h>
#include <rsa.h>
#include <memory>
#include <string>

class RSAPublicWrapper
{
public:
	static const unsigned int KEYSIZE = 160;
	static const unsigned int BITS = 1024;

private:
	CryptoPP::AutoSeededRandomPool _rng;
	CryptoPP::RSA::PublicKey _publicKey;
public:
	RSAPublicWrapper(const std::string& key);
	std::string getPublicKey() const;
	std::string encrypt(const std::string& plain);
};

typedef std::unique_ptr<RSAPublicWrapper> RSAPublicWrapperPtr;

class RSAPrivateWrapper
{
public:
	static const unsigned int BITS = 1024;

private:
	CryptoPP::AutoSeededRandomPool _rng;
	CryptoPP::RSA::PrivateKey _privateKey;
public:
	RSAPrivateWrapper();
	RSAPrivateWrapper(const std::string& key);

	std::string getPrivateKey() const;
	std::string getPublicKey() const;
	std::string decrypt(const std::string& cipher);
};

typedef std::unique_ptr<RSAPrivateWrapper> RSAPrivateWrapperPtr;
