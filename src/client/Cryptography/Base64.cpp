#include "Base64.h"


std::string b64encode(const std::vector<unsigned char>& source)
{
	std::string encoded;
	CryptoPP::VectorSource ss(source, true,
		new CryptoPP::Base64Encoder(
			new CryptoPP::StringSink(encoded)
		) // Base64Encoder
	); // StringSource

	return encoded;
}

std::vector<unsigned char> b64decode(const std::string& str)
{
	std::vector<unsigned char> decoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::VectorSink(decoded)
		) // Base64Decoder
	); // StringSource

	return decoded;
}
