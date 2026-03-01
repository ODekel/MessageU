#pragma once

#include <memory>
#include <string>

class AESWrapper
{
public:
	static const size_t DEFAULT_KEYLENGTH = 16;
private:
	std::string _key;
public:
    AESWrapper();
	AESWrapper(std::string key);

	const std::string& getKey() const;
	std::string encrypt(const char* plain, unsigned int length) const;
	std::string decrypt(const char* cipher, unsigned int length) const;
};

typedef std::unique_ptr<AESWrapper> AESWrapperPtr;
