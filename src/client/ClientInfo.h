#pragma once

#include "OtherClient.h"
#include "Cryptography/AESWrapper.h"
#include "Cryptography/RSAWrapper.h"
#include "Typedefs.h"
#include <map>
#include <memory>

class ClientInfo
{
private:
    const SocketPtr sock;
    std::string clientId;
    const uint8_t version;
    std::unordered_map<std::string, OtherClient> others;
    std::unordered_map<std::string, RSAPublicWrapperPtr> publicKeys;
    std::unordered_map<std::string, AESWrapperPtr> symmetricKeys;
public:
    ClientInfo(SocketPtr& sock, std::string clientId, uint8_t version);
    const SocketPtr& getSocket() const;
    const std::string& getClientId() const;
    uint8_t getVersion() const;
    const OtherClient& getOtherClient(std::string otherUsername) const;
    const OtherClient& getOtherClientById(std::string otherClientId) const;
    const RSAPublicWrapperPtr& getPublicKey(std::string otherClientId) const;
    const AESWrapperPtr& getSymmetricKey(std::string otherClientId) const;
    void setClientId(std::string newClientId);
    void setOthers(std::unordered_map<std::string, OtherClient> newOthers);
    void setPublicKey(std::string otherClientId, RSAPublicWrapperPtr publicKey);
    void setSymmetricKey(std::string otherClientId, AESWrapperPtr symmetricKey);
};

typedef std::unique_ptr<ClientInfo> ClientInfoPtr;
