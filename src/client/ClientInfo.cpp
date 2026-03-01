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

const OtherClient& ClientInfo::getOtherClient(std::string otherUsername) const {
    return others.at(otherUsername);
}

const OtherClient& ClientInfo::getOtherClientById(std::string otherClientId) const {
    for (const auto& [username, other] : others) {
        if (other.getClientId() == otherClientId) {
            return other;
        }
    }
    throw std::out_of_range("No client with id found.");
}

const RSAPublicWrapperPtr& ClientInfo::getPublicKey(std::string otherUsername) const {
    return publicKeys.at(otherUsername);
}

const AESWrapperPtr& ClientInfo::getSymmetricKey(std::string otherUsername) const {
    return symmetricKeys.at(otherUsername);
}

void ClientInfo::setClientId(std::string newClientId) {
    this->clientId = std::move(newClientId);
}

void ClientInfo::setOthers(std::unordered_map<std::string, OtherClient> newOthers) {
    this->others = std::move(newOthers);
}

void ClientInfo::setPublicKey(std::string otherUsername, RSAPublicWrapperPtr publicKey) {
    this->publicKeys.insert({ otherUsername, std::move(publicKey) });
}

void ClientInfo::setSymmetricKey(std::string otherUsername, AESWrapperPtr symmetricKey) {
    this->symmetricKeys.insert({ otherUsername, std::move(symmetricKey) });
}
