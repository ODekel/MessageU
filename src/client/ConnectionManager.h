#pragma once

#include "Typedefs.h"
#include <boost/asio.hpp>

class ConnectionManager
{
private:
    boost::asio::io_context _io_context;
    boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> _endpoints;
    SocketPtr _socket;
public:
    ConnectionManager(const std::string& ip, const std::string& port);
    void connect();
    void send(const std::string& buffer) const;
    void receive(std::string& buffer) const;
    void close();
};

typedef std::unique_ptr<ConnectionManager> ConnectionManagerPtr;
