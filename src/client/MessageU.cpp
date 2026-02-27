#include "Cryptography/Base64.h"
#include "Menu.h"
#include "Typedefs.h"
#include "UserInfo.h"
#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using boost::asio::ip::tcp;

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

static std::tuple<SocketPtr, std::unique_ptr<boost::asio::io_context>> connectToServer(const std::string& ip, const std::string& port) {
    auto io_context = std::unique_ptr<boost::asio::io_context>(new boost::asio::io_context());
    tcp::resolver resolver(*io_context);
    SocketPtr sock = SocketPtr(new tcp::socket(*io_context));
    try {
        boost::asio::connect(*sock, resolver.resolve(ip, port));
    } catch (const boost::system::system_error&) {
        std::cout << "Failed to connect to server " << ip << ":" << port << std::endl;
        exit(1);
    }
    return std::make_tuple(std::move(sock), std::move(io_context));
}

int main()
{
    auto [ip, port] = readIpPort("server.info");
    UserInfoPtr userInfo;
    try {
        userInfo = readUserInfo("me.info");
        std::cout << "Registered as user " << userInfo->getUsername() << "." << std::endl;
    }
    catch (const std::runtime_error&) {
        userInfo = UserInfoPtr(new UserInfo("", "", nullptr));
    }
    auto t2 = connectToServer(ip, port);
    SocketPtr sock = std::move(std::get<0>(t2));
    std::cout << sock->local_endpoint() << " connected to " << sock->remote_endpoint() << std::endl;
    ClientInfoPtr clientInfo = ClientInfoPtr(new ClientInfo(sock, std::string(16, '\0'), CLIENT_VERSION));

    std::cout << "MessageU client at your service." << std::endl;
    while (true)
        operateMenu(userInfo, clientInfo);

    return 0;
}
