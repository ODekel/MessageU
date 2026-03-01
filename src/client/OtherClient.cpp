#include "OtherClient.h"

OtherClient::OtherClient(std::string clientId, std::string username)
    : clientId(std::move(clientId)), username(std::move(username)) { }

const std::string& OtherClient::getClientId() const {
    return clientId;
}

const std::string& OtherClient::getUsername() const {
    return username;
}
