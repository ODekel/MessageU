#include "ClientInfo.h"

ClientInfo::ClientInfo(SocketPtr& sock, std::string clientId, uint8_t version)
    : sock(std::move(sock)), clientId(std::move(clientId)), version(version) { }

const SocketPtr& ClientInfo::getSocket() const {
    return sock;
}

const std::string& ClientInfo::getClientId() const {
    return clientId;
}

uint8_t ClientInfo::getVersion() const {
    return version;
}

void ClientInfo::setClientId(std::string newClientId) {
    this->clientId = std::move(newClientId);
}
