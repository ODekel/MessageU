#include "UserInfo.h"

UserInfo::UserInfo(std::string username, std::string clientId, RSAPrivateWrapperPtr privateKey)
    : username(std::move(username)), clientId(std::move(clientId)), privateKey(std::move(privateKey)) { }

const std::string& UserInfo::getUsername() const {
    return username;
}

const std::string& UserInfo::getClientId() const {
    return clientId;
}

const RSAPrivateWrapper& UserInfo::getPrivateKey() const {
    return *privateKey;
}

bool UserInfo::isRegistered() const {
    return privateKey != nullptr;
}
