#include "ServerManager.h"

namespace Networking {

using asio::ip::tcp;

ServerManager::ServerManager() = default;

ServerManager::~ServerManager() {
    std::lock_guard<std::mutex> lock(socketsMutex_);
    for (auto &sock : sockets_) {
        if (sock && sock->is_open()) {
            asio::error_code ec;
            sock->close(ec);
        }
    }
}

void ServerManager::startServer(unsigned short port)
{
    try
    {
        acceptor_ = std::make_unique<tcp::acceptor>(ioContext_, tcp::endpoint(tcp::v4(), port));
        std::cout << "[Server] Listening on port " << port << "\n";

        acceptClientsLoop();

        // Run io_context in a detached thread
        std::thread([this]() { ioContext_.run(); }).detach();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Server Error] " << e.what() << "\n";
    }
}

void ServerManager::acceptClientsLoop()
{
    auto newSock = std::make_shared<tcp::socket>(ioContext_);
    acceptor_->async_accept(*newSock, [this, newSock](std::error_code ec)
    {
        if (!ec)
        {
            {
                std::lock_guard<std::mutex> lock(socketsMutex_);
                sockets_.push_back(newSock);
            }
            std::cout << "[Server] Client connected! Total clients: " << getClientCount() << "\n";

            // Start async read/reply loop for this client
            clientSession<int>(newSock); // Replace 'int' with your type T
        }
        else
        {
            std::cerr << "[Accept Error] " << ec.message() << "\n";
        }

        // Continue accepting next clients
        acceptClientsLoop();
    });
}

template <typename T>
void ServerManager::clientSession(std::shared_ptr<tcp::socket> socket)
{
    auto buffer = std::make_shared<std::array<char, sizeof(T)>>();

    socket->async_read_some(asio::buffer(*buffer),
        [this, buffer, socket](std::error_code ec, std::size_t len)
        {
            if (ec || len != sizeof(T))
            {
                std::cerr << "[Client Error] Read failed: "
                          << (ec ? ec.message() : "wrong size") << "\n";

                // Remove disconnected client
                std::lock_guard<std::mutex> lock(socketsMutex_);
                sockets_.erase(std::remove(sockets_.begin(), sockets_.end(), socket), sockets_.end());
                return;
            }

            T object;
            std::memcpy(&object, buffer->data(), sizeof(T));

            // Process object (example: echo back)
            std::cout << "[Server] Received object\n";

            // Send reply asynchronously
            sendData(socket, object);

            // Continue the async read loop
            clientSession<T>(socket);
        });
}

} // namespace Networking