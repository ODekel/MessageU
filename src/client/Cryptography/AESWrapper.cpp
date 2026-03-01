#include "AESWrapper.h"
#include <modes.h>
#include <aes.h>
#include <filters.h>
#include <stdexcept>
#include <immintrin.h>	// _rdrand32_step

static std::string generateKey(uint32_t length)
{
    std::string buffer(length, '\0');
	for (size_t i = 0; i < length; i += sizeof(uint32_t))
		_rdrand32_step(reinterpret_cast<uint32_t*>(&buffer[i]));
	return buffer;
}

AESWrapper::AESWrapper() : _key(std::move(generateKey(DEFAULT_KEYLENGTH))) { }

AESWrapper::AESWrapper(std::string key) : _key(std::move(key))
{
	if (_key.size() != DEFAULT_KEYLENGTH) {
		throw std::length_error("Key must be " + std::to_string(DEFAULT_KEYLENGTH) + " bytes");
    }
}

const std::string& AESWrapper::getKey() const 
{ 
	return _key; 
}

std::string AESWrapper::encrypt(const char* plain, unsigned int length) const
{
	CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };	// for practical use iv should never be a fixed value!

	CryptoPP::AES::Encryption aesEncryption((uint8_t*)_key.data(), DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	std::string cipher;
	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(cipher));
	stfEncryptor.Put(reinterpret_cast<const CryptoPP::byte*>(plain), length);
	stfEncryptor.MessageEnd();

	return cipher;
}

std::string AESWrapper::decrypt(const char* cipher, unsigned int length) const
{
	CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };	// for practical use iv should never be a fixed value!

	CryptoPP::AES::Decryption aesDecryption((uint8_t*)_key.data(), DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

	std::string decrypted;
	CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decrypted));
	stfDecryptor.Put(reinterpret_cast<const CryptoPP::byte*>(cipher), length);
	stfDecryptor.MessageEnd();

	return decrypted;
}
