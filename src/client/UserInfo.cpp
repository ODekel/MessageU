#include "UserInfo.h"

UserInfo::UserInfo(std::string username, std::string clientId, RSAPrivateWrapper* privateKey)
    : username(std::move(username)), clientId(std::move(clientId)), privateKey(RSAPrivateWrapperPtr(privateKey)) { }

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
    return (bool)privateKey;
}
