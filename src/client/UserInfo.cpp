#include "UserInfo.h"

UserInfo::UserInfo(std::string username, std::vector<unsigned char> clientId, std::vector<unsigned char> privateKey)
    : username(std::move(username)), clientId(std::move(clientId)), privateKey(std::move(privateKey)) { }

const std::string& UserInfo::getUsername() const {
    return username;
}

const std::vector<unsigned char>& UserInfo::getClientId() const {
    return clientId;
}

const std::vector<unsigned char>& UserInfo::getPrivateKey() const {
    return privateKey;
}
