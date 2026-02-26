#include "MenuActions.h"
#include <cstdlib>
#include <iostream>

static constexpr int USERNAME_FIELD_SIZE = 255;

// static std::tuple<int, std::string> sendRequest(int code, const std::string& payload) {
// }

void registerUser(const UserInfoPtr& userInfo, const SocketPtr&) {
    if (userInfo->isRegistered()) {
        std::cout << "You are already registered as " << userInfo->getUsername() << "." << std::endl;
        return;
    }

    std::cout << "Enter username: ";
    std::string username;
    std::cin >> username;
    if (username.empty() || username.length() >= 255) {
        std::cout << "Username cannot be empty or over 254 characters." << std::endl;
        return;
    }
    username.insert(username.size(), USERNAME_FIELD_SIZE - username.size(), '\0');
    RSAPrivateWrapperPtr privateKey = RSAPrivateWrapperPtr(new RSAPrivateWrapper());
}

void getClientList(const UserInfoPtr&, const SocketPtr&) {
    // Code to get the client list
}

void getClientPublicKey(const UserInfoPtr&, const SocketPtr&) {
    // Code to get the client's public key
}

void getMessages(const UserInfoPtr&, const SocketPtr&) {
    // Code to get messages
}

void sendMessage(const UserInfoPtr&, const SocketPtr&) {
    // Code to send a message
}

void requestSymmetricKey(const UserInfoPtr&, const SocketPtr&) {
    // Code to request a client's public key
}

void sendSymmetricKey(const UserInfoPtr&, const SocketPtr&) {
    // Code to send a symmetric key to a client
}

void exitClient(const UserInfoPtr&, const SocketPtr&) {
    std::exit(0);
}
