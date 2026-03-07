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
        std::vector<std::shared_ptr<asio::ip::tcp::socket>> m_sockets;


        void AcceptClients();    

    public:
		template<typename T>
        T& readData(std::shared_ptr<asio::ip::tcp::socket>& fromSocket)
        {
            for (auto& socket : m_sockets) {
                asio::error_code ec;
                char buf[sizeof(T)];
                socket->async_read_some(asio::buffer(buf),
                    [&, fromSocket,socket](const asio::error_code& ec, size_t length) {

                        if (ec == asio::error::eof) {
                            std::cout << "Client disconnected: " << socket.get() << std::endl;
                            m_sockets.erase(std::remove(m_sockets.begin(), m_sockets.end(), socket), m_sockets.end());
                        }
                        else if (ec) {
                            std::cout << "Error: " << ec.message() << std::endl;
                        }
                        else {
                            T data;
                            memcpy(&data, buf, sizeof(T));
                            return data;
                            fromSocket = std::make_shared<asio::ip::tcp::socket>(socket);
                            // Process data here
                        }
                    });

            };
        }
		template<typename T>
        bool writeData(std::shared_ptr<asio::ip::tcp::socket> socket, T data)
        {
            asio::error_code ec;
            size_t len = socket->write_some(asio::buffer(&data, sizeof(T)), ec);
            if (ec == asio::error::eof) {
                std::cout << "Server closed connection." << std::endl;
                return false;
            }
            else if (ec) {
                std::cout << "Error: " << ec.message() << std::endl;
                return false;
            }
            return true;
        }
        template<typename Fd, typename Td>
        Td sendAndReceive(Fd dataToSend)
        {
			std::shared_ptr<asio::ip::tcp::socket> fromSocket;
            if (!writeData(fromSocket, dataToSend))
            {
				return readData<Td>(fromSocket);
                throw std::runtime_error("Failed to send data.");
            }
		}

        ServerManager(unsigned short port);

        ~ServerManager();
    };

    class ClientManager
    {
        asio::io_context m_ioContext;
        std::unique_ptr<asio::ip::tcp::socket> m_socket;
    public:
		template<typename T>
        T& readData()
        {
            asio::error_code ec;
            char buf[sizeof(T)];
            size_t len = m_socket->read_some(asio::buffer(buf), ec);
            if (ec == asio::error::eof) {
                std::cout << "Server closed connection." << std::endl;
                return 0;
            }
            else if (ec) {
                std::cout << "Error: " << ec.message() << std::endl;
                return 0;
            }
            T data;
            memcpy(&data, buf, sizeof(T));
            return data;
        }
        template<typename T>
        bool writeData(T data)
        {
            asio::error_code ec;
            size_t len = m_socket->write_some(asio::buffer(&data, sizeof(T)), ec);
            if (ec == asio::error::eof) {
                std::cout << "Server closed connection." << std::endl;
                return false;
            }
            else if (ec) {
                std::cout << "Error: " << ec.message() << std::endl;
                return false;
            }
            return true;
        }
        template<typename Fd,typename Td>
        Td sendAndReceive(Fd dataToSend)
        {
            if (!writeData(dataToSend))
            {
                throw std::runtime_error("Failed to send data.");
            }
            return readData<Td>();
		}
        ClientManager(const char* ip, unsigned short port);
        ~ClientManager();
    };

} // namespace NetworkManagement
