#include "ActionHelpers.h"
#include "Cryptography/Base64.h"
#include "MenuActions.h"
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

void registerUser(const UserInfoPtr& userInfo, const ClientInfoPtr& clientInfo) {
    if (userInfo->isRegistered()) {
        std::cout << "You are already registered as " << userInfo->getUsername().substr(0, userInfo->getUsername().find('\0')) << "." << std::endl;
        return;
    }

    std::cout << "Enter username: ";
    std::string username;
    std::cin >> username;
    if (username.empty() || username.length() >= USERNAME_FIELD_SIZE) {
        std::cout << "Username cannot be empty or over 254 characters." << std::endl;
        return;
    }
    username.insert(username.size(), USERNAME_FIELD_SIZE - username.size(), '\0');
    RSAPrivateWrapper* privateKey = new RSAPrivateWrapper();

    auto [resCode, resPayload] = sendRequest(700, username + privateKey->getPublicKey(), clientInfo);
    if (handleError(resCode)) return;

    std::string hexId = uuidToHex(resPayload);
    std::ofstream userInfoFile(USER_INFO_PATH);
    userInfoFile << username << std::endl
        << hexId << std::endl
        << b64encode(privateKey->getPrivateKey());
    userInfoFile.close();

    *userInfo = std::move(UserInfo(username, resPayload, privateKey));
    clientInfo->setClientId(resPayload.substr());
    std::cout << "Registered successfully as " << userInfo->getUsername() << "." << std::endl;
}

void getClientList(const UserInfoPtr& userInfo, const ClientInfoPtr& clientInfo) {
    if (!checkRegistration(userInfo)) return;
    auto [resCode, resPayload] = sendRequest(701, "", clientInfo);
    if (handleError(resCode)) return;

    int amount = resPayload.size() / SINGLE_CLIENT_PAYLOAD_SIZE;
    std::unordered_map<std::string, OtherClient> others;
    if (amount == 0) {
        std::cout << "No other clients found." << std::endl;
        return;
    } else {
        std::cout << "Other clients:" << std::endl;
        others.reserve(amount);
    }
    for (size_t i = 0; i < resPayload.size(); i += SINGLE_CLIENT_PAYLOAD_SIZE) {
        std::string rawUsername = resPayload.substr(i + 16, USERNAME_FIELD_SIZE);
        std::string username = rawUsername.substr(0, rawUsername.find('\0'));
        OtherClient other = OtherClient(resPayload.substr(i, 16), username);
        others.insert({ username, other });
        std::cout << username << std::endl;
    }
    clientInfo->setOthers(others);
}

void getClientPublicKey(const UserInfoPtr& userInfo, const ClientInfoPtr& clientInfo) {
    if (!checkRegistration(userInfo)) return;

    std::string targetUsername;
    std::cout << "Enter the username of the client you want to get the public key of: ";
    std::cin >> targetUsername;
    const OtherClient* targetClient;
    try {
        targetClient = &clientInfo->getOtherClient(targetUsername);
    } catch (const std::out_of_range&) {
        std::cout << "No client with username " << targetUsername << " found." << std::endl;
        return;
    }
    auto [resCode, resPayload] = sendRequest(702, targetClient->getClientId(), clientInfo);
    if (handleError(resCode)) return;

    std::string targetClientId = resPayload.substr(0, 16);
    RSAPublicWrapperPtr publicKey(new RSAPublicWrapper(resPayload.substr(16, 160)));
    clientInfo->setPublicKey(targetClientId, std::move(publicKey));
    std::cout << "Public key of " << targetUsername << " obtained successfully." << std::endl;
}

void getMessages(const UserInfoPtr& userInfo, const ClientInfoPtr& clientInfo) {
    if (!checkRegistration(userInfo)) return;
    auto [resCode, resPayload] = sendRequest(704, "", clientInfo);
    if (handleError(resCode)) return;

    if (resPayload.empty()) {
        std::cout << "No new messages." << std::endl;
        return;
    }
    std::cout << "Messages:" << std::endl << std::endl;
    size_t loc = 0;
    auto payloadAddr = resPayload.data();
    while (loc < resPayload.size()) {
        std::string senderId = resPayload.substr(loc, 16);
        loc += 16;
        loc += 4; // Skip message id
        MessageType messageType = (MessageType)resPayload[loc++];
        uint32_t messageSize = network_to_host_long(*(uint32_t*)(payloadAddr + loc));
        loc += 4;
        std::string message = resPayload.substr(loc, messageSize);
        loc += messageSize;

        std::string from;
        try {
            from = clientInfo->getOtherClientById(senderId).getUsername();
        } catch (const std::out_of_range&) {
            from = "UUID: " + uuidToHex(senderId);
        }

        std::string content;
        switch (messageType) {
            case SYMMETRIC_KEY_REQUEST:
                content = "Request for symmetric key";
                break;
            case SYMMETRIC_KEY_SEND:
                try {
                    saveSymmetricKey(message, senderId, clientInfo, userInfo);
                    content = "Symmetric key received";
                } catch (const std::out_of_range&) {
                    content = "can't decrypt symmetric key";
                }
                break;
            case TEXT:
                try {
                    content = decryptTextMessage(message, senderId, clientInfo);
                } catch (const std::out_of_range&) {
                    content = "can't decrypt message";
                }
                break;
            default:
                content = "Unknown message type";
        }

        std::cout << "From: " << from  << std::endl
            << "Content:" << std::endl
            << content << std::endl
            << "-----<EOM>-----" << std::endl << std::endl;
    }
}

void sendTextMessage(const UserInfoPtr& userInfo, const ClientInfoPtr& clientInfo) {
    if (!checkRegistration(userInfo)) return;

    std::cout << "Send message to: ";
    std::string targetUsername;
    std::cin >> targetUsername;
    std::string targetId;
    try {
        targetId = clientInfo->getOtherClient(targetUsername).getClientId();
    } catch (const std::out_of_range&) {
        std::cout << "No client with username " << targetUsername << " found." << std::endl;
        return;
    }
    const AESWrapperPtr* symKey;
    try {
        symKey = &clientInfo->getSymmetricKey(targetId);
    } catch (const std::out_of_range&) {
        std::cout << "No symmetric key for client " << targetUsername << " found." << std::endl;
        return;
    }

    std::cout << "Enter message: ";
    std::string message;
    std::cin >> message;

    std::string payload = symKey->get()->encrypt(message.data(), message.size());

    if (sendMessage(targetId, TEXT, payload, clientInfo))
        std::cout << "Message sent." << std::endl;
}

void requestSymmetricKey(const UserInfoPtr& userInfo, const ClientInfoPtr& clientInfo) {
    if (!checkRegistration(userInfo)) return;

    std::cout << "Send symmetric key request to: ";
    std::string targetUsername;
    std::cin >> targetUsername;
    std::string targetId;
    try {
        targetId = clientInfo->getOtherClient(targetUsername).getClientId();
    } catch (const std::out_of_range&) {
        std::cout << "No client with username " << targetUsername << " found." << std::endl;
        return;
    }

    if (sendMessage(targetId, SYMMETRIC_KEY_REQUEST, "", clientInfo))
        std::cout << "Symmetric key request sent." << std::endl;
}

void sendSymmetricKey(const UserInfoPtr& userInfo, const ClientInfoPtr& clientInfo) {
    if (!checkRegistration(userInfo)) return;

    std::cout << "Send symmetric key to: ";
    std::string targetUsername;
    std::cin >> targetUsername;
    std::string targetId;
    try {
        targetId = clientInfo->getOtherClient(targetUsername).getClientId();
    } catch (const std::out_of_range&) {
        std::cout << "No client with username " << targetUsername << " found." << std::endl;
        return;
    }
    const RSAPublicWrapperPtr* publicKey;
    try {
        publicKey = &clientInfo->getPublicKey(targetId);
    } catch (const std::out_of_range&) {
        std::cout << "No public key for client " << targetUsername << " found." << std::endl;
        return;
    }

    AESWrapper* symKey = new AESWrapper();
    if (sendMessage(targetId, SYMMETRIC_KEY_SEND, publicKey->get()->encrypt(symKey->getKey()), clientInfo)) {
        clientInfo->setSymmetricKey(targetId, AESWrapperPtr(symKey));
        std::cout << "Symmetric key sent." << std::endl;
    }
}

void exitClient(const UserInfoPtr&, const ClientInfoPtr&) {
    std::exit(0);
}
