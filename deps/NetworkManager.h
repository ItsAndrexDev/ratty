#pragma once
#define ASIO_STANDALONE
#include "asio/asio.hpp"
#include <memory>
#include <thread>
#include <iostream>
#include <mutex>
#include <list>

namespace Crilly
{
    // Wrapper for Server-side to track which socket sent the data
    template<typename T>
    struct IncomingMessage {
        std::shared_ptr<asio::ip::tcp::socket> socket;
        T data;
        IncomingMessage() {};
        IncomingMessage(T data) : data(data) {};
    };

    class ServerManager {
        asio::io_context m_ioContext;
        std::unique_ptr<asio::ip::tcp::acceptor> m_acceptor;
        std::list<std::shared_ptr<asio::ip::tcp::socket>> m_sockets;
        std::mutex m_mutex;
        std::thread m_workerThread;
        bool m_running = true;

        void Update();

    public:
        ServerManager(unsigned short port);
        ~ServerManager();

        template<typename T>
        void writeData(std::shared_ptr<asio::ip::tcp::socket> sock, const T& data) {
            if (!sock || !sock->is_open()) return;
            asio::write(*sock, asio::buffer(&data, sizeof(T)));
        }

        template<typename T>
        bool getNextMessage(IncomingMessage<T>& out) {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto& sock : m_sockets) {
                if (sock->is_open() && sock->available() >= sizeof(T)) {
                    asio::error_code ec;
                    asio::read(*sock, asio::buffer(&out.data, sizeof(T)), ec);
                    if (!ec) {
                        out.socket = sock;
                        return true;
                    }
                }
            }
            return false;
        }
    };

    class ClientManager {
        asio::io_context m_ioContext;
        std::shared_ptr<asio::ip::tcp::socket> m_socket;

    public:
        ClientManager(const char* ip, unsigned short port);
        ~ClientManager();

        template<typename T>
        void writeData(const T& obj) {
            if (m_socket && m_socket->is_open())
                asio::write(*m_socket, asio::buffer(&obj, sizeof(T)));
        }

        // Non-blocking check for new data
        // USED WITH WHILE FUNCTION
        template<typename T>
        bool getNextMessage(IncomingMessage<T>& out) {
            if (m_socket->is_open() && m_socket->available() >= sizeof(T)) {
                asio::error_code ec;
                asio::read(*m_socket, asio::buffer(&out.data, sizeof(T)), ec);
                if (!ec) {
                    out.socket = m_socket;
                    return true;
                }
            }
            return false;
        }

        bool isConnected() { return m_socket && m_socket->is_open(); }
    };
}