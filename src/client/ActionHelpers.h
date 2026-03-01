#pragma once

#include "ClientInfo.h"
#include "UserInfo.h"
#include "Cryptography/AESWrapper.h"
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <tuple>
#include <string>

// Halper function for menu actions.

using namespace boost::asio::detail::socket_ops;
using boost::asio::ip::tcp;

static constexpr int USERNAME_FIELD_SIZE = 255;
static constexpr int ERROR_RESPONSE_CODE = 9000;
static constexpr int RESPONSE_HEADER_SIZE = 7;
static constexpr int SINGLE_CLIENT_PAYLOAD_SIZE = 16 + 255;
static constexpr int MESSAGE_HEADER_SIZE = 16 + 1 + 4;
static const std::tuple<int, std::string> ERROR_RV = std::make_tuple(ERROR_RESPONSE_CODE, "");
static const std::string USER_INFO_PATH = "./me.info";

enum MessageType : uint8_t {
    SYMMETRIC_KEY_REQUEST = 1,
    SYMMETRIC_KEY_SEND = 2,
    TEXT = 3
};

static std::string uuidToHex(std::string uuid) {
    boost::uuids::uuid id;
    std::memcpy(id.data, uuid.data(), 16);
    std::string hexId = boost::uuids::to_string(id);
    hexId.erase(std::remove(hexId.begin(), hexId.end(), '-'), hexId.end());
    return hexId;
}

// Read function that doesn't throw on fail.
static bool safeRead(tcp::socket& s, const boost::asio::mutable_buffer& buffers) {
    try {
        boost::asio::read(s, buffers);
        return true;
    }
    catch (const boost::system::system_error&) {
        return false;
    }
}

// Write function that doesn't throw on fail.
static bool safeWrite(tcp::socket& s, const boost::asio::const_buffer& buffers) {
    try {
        boost::asio::write(s, buffers);
        return true;
    }
    catch (const boost::system::system_error&) {
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

// Handle server error response for the menu action functions.
static bool handleError(int resCode) {
    if (resCode == ERROR_RESPONSE_CODE) {
        std::cout << "Server responded with an error." << std::endl;
        return true;
    }
    return false;
}

static bool checkRegistration(const UserInfoPtr& userInfo) {
    if (!userInfo->isRegistered()) {
        std::cout << "Please register before using the client." << std::endl;
        return false;
    }
    return true;
}

static void saveSymmetricKey(const std::string& encKey, const std::string& otherClientId,
                             const ClientInfoPtr& clientInfo, const UserInfoPtr& userInfo) {
    clientInfo->setSymmetricKey(otherClientId, AESWrapperPtr(
        new AESWrapper(userInfo->getPrivateKey().decrypt(encKey))
    ));
}

static std::string decryptTextMessage(const std::string& cipher, const std::string& senderId,
                                      const ClientInfoPtr& clientInfo) {
    return clientInfo->getSymmetricKey(senderId)->decrypt(cipher.data(), cipher.size());
}

static bool sendMessage(const std::string& to, MessageType type, const std::string& payload,
                        const ClientInfoPtr& clientInfo) {
    std::string headers(MESSAGE_HEADER_SIZE, '\0');
    headers.replace(0, 16, to);
    headers[16] = type;
    uint32_t size_net = host_to_network_long(payload.size());
    headers.replace(17, 4, std::string((char*)&size_net, 4));
    auto [resCode, resPayload] = sendRequest(703, headers + payload, clientInfo);
    return !handleError(resCode);
}
