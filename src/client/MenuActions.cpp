#include "Cryptography/Base64.h"
#include "MenuActions.h"
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace boost::asio::detail::socket_ops;
using boost::asio::ip::tcp;

static constexpr int USERNAME_FIELD_SIZE = 255;
static constexpr int ERROR_RESPONSE_CODE = 9000;
static constexpr int RESPONSE_HEADER_SIZE = 7;
static const std::tuple<int, std::string> ERROR_RV = std::make_tuple(ERROR_RESPONSE_CODE, "");
static const std::string USER_INFO_PATH = "./me.info";

static bool safeRead(tcp::socket& s, const boost::asio::mutable_buffer& buffers) {
    try {
        boost::asio::read(s, buffers);
        return true;
    } catch (const boost::system::system_error&) {
        return false;
    }
}

static bool safeWrite(tcp::socket& s, const boost::asio::const_buffer& buffers) {
    try {
        boost::asio::write(s, buffers);
        return true;
    } catch (const boost::system::system_error&) {
        return false;
    }
}

static std::tuple<int, std::string> sendRequest(uint16_t code, const std::string& payload, const ClientInfoPtr& clientInfo) {
    std::string header(clientInfo->getClientId());
    uint8_t version = clientInfo->getVersion();
    header.append((char*)&version, 1);
    uint16_t code_net = host_to_network_short(code);
    header.append((char*)&code_net, 2);
    uint32_t size_net = host_to_network_long(payload.size());
    header.append((char*)&size_net, 4);
    if (!safeWrite(*clientInfo->getSocket(), boost::asio::buffer(header + payload))) {
        return ERROR_RV;
    };
    char responseHeaders[RESPONSE_HEADER_SIZE];
    if (!safeRead(*clientInfo->getSocket(), boost::asio::buffer(responseHeaders))) {
        return ERROR_RV;
    }
    uint16_t resCode = network_to_host_short(*(uint16_t*)(responseHeaders + 1));
    uint32_t resSize = network_to_host_long(*(uint32_t*)(responseHeaders + 3));
    std::string resPayload(resSize, '\0');
    if (resSize > 0 && !safeRead(*clientInfo->getSocket(), boost::asio::buffer(resPayload))) {
        return ERROR_RV;
    }
    return std::make_tuple(resCode, resPayload);
}

static bool handleError(int resCode) {
    if (resCode == ERROR_RESPONSE_CODE) {
        std::cout << "Server responded with an error." << std::endl;
        return true;
    }
    return false;
}

void registerUser(const UserInfoPtr& userInfo, const ClientInfoPtr& clientInfo) {
    if (userInfo->isRegistered()) {
        std::cout << "You are already registered as " << userInfo->getUsername() << "." << std::endl;
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

    boost::uuids::uuid id;
    std::memcpy(id.data, resPayload.data(), 16);
    std::string hexId = boost::uuids::to_string(id);
    hexId.erase(std::remove(hexId.begin(), hexId.end(), '-'), hexId.end());
    std::ofstream userInfoFile(USER_INFO_PATH);
    userInfoFile << username << std::endl
        << hexId << std::endl
        << b64encode(privateKey->getPrivateKey());
    userInfoFile.close();

    *userInfo = std::move(UserInfo(username, resPayload, privateKey));
    clientInfo->setClientId(resPayload);
    std::cout << "Registered successfully as " << userInfo->getUsername() << "." << std::endl;
}

void getClientList(const UserInfoPtr&, const ClientInfoPtr&) {
    // Code to get the client list
}

void getClientPublicKey(const UserInfoPtr&, const ClientInfoPtr&) {
    // Code to get the client's public key
}

void getMessages(const UserInfoPtr&, const ClientInfoPtr&) {
    // Code to get messages
}

void sendMessage(const UserInfoPtr&, const ClientInfoPtr&) {
    // Code to send a message
}

void requestSymmetricKey(const UserInfoPtr&, const ClientInfoPtr&) {
    // Code to request a client's public key
}

void sendSymmetricKey(const UserInfoPtr&, const ClientInfoPtr&) {
    // Code to send a symmetric key to a client
}

void exitClient(const UserInfoPtr&, const ClientInfoPtr&) {
    std::exit(0);
}
