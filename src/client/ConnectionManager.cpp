#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(const std::string& ip, const std::string& port)
    : _io_context(), _endpoints(boost::asio::ip::tcp::resolver(_io_context).resolve(ip, port)), _socket(nullptr) { }

void ConnectionManager::connect() {
    _socket.reset(new boost::asio::ip::tcp::socket(_io_context));
    boost::asio::connect(*_socket, _endpoints);
}

void ConnectionManager::send(const std::string& buffer) const {
    boost::asio::write(*_socket, boost::asio::buffer(buffer));
}

void ConnectionManager::receive(std::string& buffer) const {
    boost::asio::read(*_socket, boost::asio::buffer(buffer));
}

void ConnectionManager::close() {
    if (_socket) {
        _socket->close();
        _socket.reset(nullptr);
    }
}
