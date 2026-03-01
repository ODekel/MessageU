#pragma once

#include <string>

class OtherClient
{
private:
    std::string clientId;
    std::string username;
public:
    OtherClient(std::string clientId, std::string username);
    const std::string& getClientId() const;
    const std::string& getUsername() const;
};

