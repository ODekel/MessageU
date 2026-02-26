#include "MenuActions.h"
#include <cstdlib>
#include <iostream>

void registerUser(const UserInfoPtr& userInfo) {
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


}

void getClientList(const UserInfoPtr&) {
    // Code to get the client list
}

void getClientPublicKey(const UserInfoPtr&) {
    // Code to get the client's public key
}

void getMessages(const UserInfoPtr&) {
    // Code to get messages
}

void sendMessage(const UserInfoPtr&) {
    // Code to send a message
}

void requestSymmetricKey(const UserInfoPtr&) {
    // Code to request a client's public key
}

void sendSymmetricKey(const UserInfoPtr&) {
    // Code to send a symmetric key to a client
}

void exitClient(const UserInfoPtr&) {
    std::exit(0);
}
