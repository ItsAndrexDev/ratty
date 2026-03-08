#include "NetworkManager.h"

#include <iostream>

using namespace NetworkManagement;
using namespace asio::ip;

ServerManager::ServerManager(unsigned short port)
{
    m_acceptor = std::make_unique<tcp::acceptor>(
        m_ioContext,
        tcp::endpoint(tcp::v4(), port)
    );
	m_ioContext.run();
    std::cout << "ioContext.run()!" << std::endl;
	RemoveDisconnected();
	AcceptClients();
    std::cout << "Server started on port " << port << std::endl;
}


ServerManager::~ServerManager()
{

}






void ServerManager::AcceptClients() {

    std::thread([this]() {
        for (;;) {
            auto socket = std::make_shared<asio::ip::tcp::socket>(m_ioContext);
            m_acceptor->accept(*socket);

            socket->set_option(asio::socket_base::keep_alive(true));
            std::cout << "Client connected: " << socket.get() << std::endl;
            m_sockets.push_back(socket);
        }
    }).detach();
}


ClientManager::ClientManager(const char* ip, unsigned short port) : m_socket(std::make_unique<asio::ip::tcp::socket>(m_ioContext))
{
    try {
        tcp::resolver resolver(m_ioContext);
        auto endpoints = resolver.resolve(ip, std::to_string(port));
        asio::connect(*m_socket, endpoints);
        std::cout << "[Client] Connected to server " << ip << ":" << port << "\n";
    }
    catch (std::exception& e) {
        std::cerr << "[Client Error] " << e.what() << "\n";
    }
}

ClientManager::~ClientManager()
{
}
