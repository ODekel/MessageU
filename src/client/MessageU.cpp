#include "Cryptography/Base64.h"
#include "Menu.h"
#include "UserInfo.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

std::tuple<std::string, std::string> readIpPort(std::string path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string ip, port;
    std::getline(buffer, ip, ':');
    std::getline(buffer, port);
    if (ip.empty() || port.empty()) {
        std::cout << "Invalid ip or port file " << path << std::endl;
        exit(1);
    }
    return { ip, port };
}

std::shared_ptr<UserInfo> readUserInfo(std::string path) {
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
    if (username.empty() || id_hex.empty() || symmetricKey_base64.empty()) {
        std::cout << "Invalid user info file " << path << std::endl;
        exit(1);
    }
    std::istringstream id_stream(id_hex);
    std::vector<unsigned char> id(16);
    for (int i = 0; i < 16; ++i) {
        id_stream >> std::hex >> id[i];
    }
    return std::shared_ptr<UserInfo>(new UserInfo(username, id, b64decode(symmetricKey_base64)));
}

int main()
{
    auto t1 = readIpPort("server.info");
    std::string ip = std::get<0>(t1);
    std::string port = std::get<1>(t1);
    std::shared_ptr<UserInfo> userInfo;
    try {
        userInfo = readUserInfo("me.info");
    }
    catch (const std::runtime_error& e) {
        userInfo = nullptr;
    }

    std::cout << "MessageU client at your service." << std::endl;
    operateMenu();
    return 0;
}
