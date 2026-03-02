#include "Cryptography/Base64.h"
#include "Menu.h"
#include "UserInfo.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

static constexpr uint8_t CLIENT_VERSION = 1;

static std::tuple<std::string, std::string> readIpPort(std::string path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string ip, port;
    std::getline(buffer, ip, ':');
    std::getline(buffer, port);
    file.close();
    if (ip.empty() || port.empty()) {
        std::cout << "Invalid ip or port file " << path << std::endl;
        exit(1);
    }
    return { ip, port };
}

static UserInfoPtr readUserInfo(std::string path) {
    std::ifstream file(path);
    if (!file.good()) {
        throw std::runtime_error("User info file not found: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string username, id_hex, symmetricKey_base64;
    std::getline(buffer, username);
    std::getline(buffer, id_hex);
    std::getline(buffer, symmetricKey_base64);
    file.close();
    if (username.empty() || id_hex.empty() || symmetricKey_base64.empty()) {
        std::cout << "Invalid user info file " << path << std::endl;
        exit(1);
    }
    std::string id;
    id.reserve(16);
    for (int i = 0; i < 32; i += 2) {
        id += static_cast<char>(std::stoi(id_hex.substr(i, 2), nullptr, 16));
    }
    return UserInfoPtr(new UserInfo(username, id, new RSAPrivateWrapper(b64decode(symmetricKey_base64))));
}

int main()
{
    auto [ip, port] = readIpPort("server.info");
    UserInfoPtr userInfo;
    try {
        userInfo = readUserInfo("me.info");
        std::cout << "Registered as user " << userInfo->getUsername().substr(0, userInfo->getUsername().find('\0')) << "." << std::endl;
    }
    catch (const std::runtime_error&) {
        userInfo = UserInfoPtr(new UserInfo("", std::string(16, '\0'), nullptr));
    }
    ConnectionManagerPtr connMgr(new ConnectionManager(ip, port));
    ClientInfoPtr clientInfo = ClientInfoPtr(new ClientInfo(connMgr, userInfo->getClientId(), CLIENT_VERSION));

    std::cout << "MessageU client at your service." << std::endl;
    while (true)
        operateMenu(userInfo, clientInfo);

    return 0;
}
