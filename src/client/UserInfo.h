#pragma once

#include <string>
#include <vector>

class UserInfo
{
private:
    std::string username;
    std::vector<unsigned char> clientId;
    std::vector<unsigned char> privateKey;
public:
    UserInfo(std::string username, std::vector<unsigned char> clientId, std::vector<unsigned char> privateKey);
    const std::string& getUsername() const;
    const std::vector<unsigned char>& getClientId() const;
    const std::vector<unsigned char>& getPrivateKey() const;
};

