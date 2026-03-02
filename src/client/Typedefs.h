#pragma once

#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

typedef std::unique_ptr<tcp::socket> SocketPtr;
