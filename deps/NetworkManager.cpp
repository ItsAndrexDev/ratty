#include "NetworkManager.h"

using namespace Crilly;
using asio::ip::tcp;

ServerManager::ServerManager(unsigned short port) {
    m_acceptor = std::make_unique<tcp::acceptor>(m_ioContext, tcp::endpoint(tcp::v4(), port));
    m_acceptor->non_blocking(true); // Don't block the management thread

    m_workerThread = std::thread([this]() { Update(); });
    std::cout << "Server initialized on port " << port << std::endl;
}

ServerManager::~ServerManager() {
    m_running = false;
    if (m_workerThread.joinable()) m_workerThread.join();
}

void ServerManager::Update() {
    while (m_running) {
        // 1. Handle New Connections
        asio::error_code ec;
        auto socket = std::make_shared<tcp::socket>(m_ioContext);
        m_acceptor->accept(*socket, ec);

        if (!ec) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sockets.push_back(socket);
            std::cout << "New client connected.\n";
        }

        // 2. Clean up dead sockets
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sockets.remove_if([](auto& s) { return !s->is_open(); });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


ClientManager::ClientManager(const char* ip, unsigned short port)
{
    m_socket = std::make_shared<tcp::socket>(m_ioContext);

    tcp::resolver resolver(m_ioContext);
    auto endpoints = resolver.resolve(ip, std::to_string(port));

    asio::connect(*m_socket, endpoints);

    std::cout << "Connected to server\n";
}

ClientManager::~ClientManager()
{
    if (m_socket && m_socket->is_open())
        m_socket->close();
    m_socket.reset();
}