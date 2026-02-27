#pragma once

#include "Typedefs.h"
#include <memory>

class ClientInfo
{
private:
    const SocketPtr sock;
    std::string clientId;
    const uint8_t version;
public:
    ClientInfo(SocketPtr& sock, std::string clientId, uint8_t version);
    const SocketPtr& getSocket() const;
    const std::string& getClientId() const;
    uint8_t getVersion() const;
    void setClientId(std::string newClientId);
};

typedef std::unique_ptr<ClientInfo> ClientInfoPtr;
