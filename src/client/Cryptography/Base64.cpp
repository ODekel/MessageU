#include "Base64.h"


std::string b64encode(const std::string& source)
{
	std::string encoded;
	CryptoPP::StringSource ss(source, true,
		new CryptoPP::Base64Encoder(
			new CryptoPP::StringSink(encoded)
		) // Base64Encoder
	); // StringSource

	return encoded;
}

std::string b64decode(const std::string& source)
{
	std::string decoded;
	CryptoPP::StringSource ss(source, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(decoded)
		) // Base64Decoder
	); // StringSource

	return decoded;
}
