#pragma once
#define ASIO_STANDALONE
#include "asio/asio.hpp"
#include <memory>
#include <thread>
#include <iostream>
namespace NetworkManagement
{
    enum class NetworkError
    {
        ConnectionFailed = 0,
        SendFailed,
        ReceiveFailed,
        UnknownError
	};

    class ServerManager
    {
        asio::io_context m_ioContext;
        std::unique_ptr<asio::ip::tcp::acceptor> m_acceptor;

        void RemoveDisconnected() {
            std::thread([this]() {
                while (true) {
                    m_sockets.erase(
                        std::remove_if(
                            m_sockets.begin(),
                            m_sockets.end(),
                            [](const auto& s) { return s->is_open(); }),
                        m_sockets.end()
                    );
					std::this_thread::sleep_for(std::chrono::seconds(5));
                }
				}).detach();
        }
        void AcceptClients();    
        std::vector<std::shared_ptr<asio::ip::tcp::socket>> m_sockets;

    public:
        template<typename T, typename Handler>
        void readData(Handler handler)
        {
            for (auto& socket : m_sockets)
            {
                auto buf = std::make_shared<std::array<char, sizeof(T)>>();

                socket->async_read_some(
                    asio::buffer(*buf),
                    [&, socket, buf, handler](const asio::error_code& ec, size_t length)
                    {
                        if (ec == asio::error::eof)
                        {
                            std::cout << "Client disconnected: " << socket.get() << std::endl;
                            return;
                        }
                        if (ec)
                        {
                            std::cout << "Error: " << ec.message() << std::endl;
                            return;
                        }

                        if (length >= sizeof(T))
                        {
                            T data;
                            memcpy(&data, buf->data(), sizeof(T));

                            handler(data, socket);  // deliver result
                        }
                    });
            }
        }
        template<typename T, typename Handler>
        void writeData(std::shared_ptr<asio::ip::tcp::socket> socket, const T& data, Handler handler)
        {
            auto buffer = std::make_shared<T>(data);

            asio::async_write(
                *socket,
                asio::buffer(buffer.get(), sizeof(T)),
                [socket, buffer, handler](const asio::error_code& ec, std::size_t length)
                {
                    if (ec == asio::error::eof)
                    {
                        std::cout << "Server closed connection." << std::endl;
                        handler(false);
                        return;
                    }

                    if (ec)
                    {
                        std::cout << "Error: " << ec.message() << std::endl;
                        handler(false);
                        return;
                    }

                    handler(true);
                });
        }

        ServerManager(unsigned short port);

        ~ServerManager();
    };

    class ClientManager
    {
        asio::io_context m_ioContext;
        std::shared_ptr<asio::ip::tcp::socket> m_socket;
    public:
        template<typename T, typename Handler>
        void readData(Handler handler)
        {
            auto buf = std::make_shared<std::array<char, sizeof(T)>>();

            m_socket->async_read_some(
                asio::buffer(*buf),
                [&, this, buf, handler](const asio::error_code& ec, size_t length)
                {
                    if (ec == asio::error::eof)
                    {
                        std::cout << "Client disconnected" << std::endl;
                        return;
                    }
                    if (ec)
                    {
                        std::cout << "Error: " << ec.message() << std::endl;
                        return;
                    }

                    if (length >= sizeof(T))
                    {
                        T data;
                        memcpy(&data, buf->data(), sizeof(T));

                        handler(data, m_socket);  // deliver result
                    }
                });
            
        }
        template<typename T, typename Handler>
        void writeData(const T& data, Handler handler)
        {
            auto buffer = std::make_shared<T>(data);

            asio::async_write(
                *m_socket,
                asio::buffer(buffer.get(), sizeof(T)),
                [handler](const asio::error_code& ec, std::size_t length)
                {
                    if (ec == asio::error::eof)
                    {
                        std::cout << "Server closed connection." << std::endl;
                        handler(false);
                        return;
                    }

                    if (ec)
                    {
                        std::cout << "Error: " << ec.message() << std::endl;
                        handler(false);
                        return;
                    }

                    handler(true);
                });
        }
        ClientManager(const char* ip, unsigned short port);
        ~ClientManager();
    };

} // namespace NetworkManagement
