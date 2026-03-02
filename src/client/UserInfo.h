#pragma once

#include "Cryptography/RSAWrapper.h"
#include <memory>
#include <string>

// Holds information about the user that is saved between client runs.

class UserInfo
{
private:
    std::string username;
    std::string clientId;
    RSAPrivateWrapperPtr privateKey;
public:
    UserInfo(std::string username, std::string clientId, RSAPrivateWrapper* privateKey);
    const std::string& getUsername() const;
    const std::string& getClientId() const;
    const RSAPrivateWrapper& getPrivateKey() const;
    bool isRegistered() const;
};

typedef std::unique_ptr<UserInfo> UserInfoPtr;
