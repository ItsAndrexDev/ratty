#pragma once

#include "asio/asio.hpp"
#include <vector>
#include <memory>
#include <array>
#include <iostream>
#include <mutex>
#include <functional>
#include <algorithm>

namespace Networking {

class ServerManager
{
public:
    ServerManager();
    ~ServerManager();

    // Start listening for clients (non-blocking)
    void startServer(unsigned short port);

    // Broadcast a message to all connected clients asynchronously
    template <typename T>
    void broadcastData(const T &data)
    {
        std::lock_guard<std::mutex> lock(socketsMutex_);
        for (auto &sock : sockets_)
        {
            if (sock && sock->is_open())
            {
                sendData(sock, data);
            }
        }
    }

    // Send data asynchronously to a specific client
    template <typename T>
    void sendData(std::shared_ptr<asio::ip::tcp::socket> socket, const T &message)
    {
        auto buffer = std::make_shared<std::array<char, sizeof(T)>>();
        std::memcpy(buffer->data(), &message, sizeof(T));

        asio::async_write(*socket,
                          asio::buffer(*buffer),
                          [buffer](std::error_code ec, std::size_t)
                          {
                              if (ec)
                                  std::cerr << "[Send Error] " << ec.message() << "\n";
                          });
    }

    // Return number of connected clients
    size_t getClientCount()
    {
        std::lock_guard<std::mutex> lock(socketsMutex_);
        return sockets_.size();
    }

private:
    void acceptClientsLoop(); // Async accept loop

    template <typename T>
    void clientSession(std::shared_ptr<asio::ip::tcp::socket> socket); // Async read/reply loop

    std::vector<std::shared_ptr<asio::ip::tcp::socket>> sockets_;
    asio::io_context ioContext_;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    std::mutex socketsMutex_;
};

} // namespace Networking