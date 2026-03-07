#pragma once
#include "asio/asio.hpp"
#include <memory>
#include <array>
#include <iostream>
#include <functional>

namespace Networking
{

    struct Message
    {
        int value;
    };

    class ClientManager
    {
    public:
        ClientManager() : socket_(std::make_shared<asio::ip::tcp::socket>(ioContext_)) {};

        ~ClientManager();

        // Fully async connect
        void connect(const std::string &host, unsigned short port,
                     std::function<void(bool)> onConnect)
        {
            auto resolver = std::make_shared<asio::ip::tcp::resolver>(ioContext_);
            resolver->async_resolve(host, std::to_string(port),
                                    [this, resolver, onConnect](std::error_code ec,
                                                                asio::ip::tcp::resolver::results_type endpoints)
                                    {
                                        if (ec)
                                        {
                                            std::cerr << "[Resolve Error] " << ec.message() << "\n";
                                            onConnect(false);
                                            return;
                                        }

                                        asio::async_connect(*socket_, endpoints,
                                                            [this, onConnect](std::error_code ec, asio::ip::tcp::endpoint)
                                                            {
                                                                if (!ec)
                                                                {
                                                                    std::cout << "[Client] Connected!\n";
                                                                    onConnect(true);

                                                                    // Start the async read/write loop automatically
                                                                    startAsyncLoop<Message>([](const Message &msg)
                                                                                            { std::cout << "Server sent: " << msg.value << "\n"; });
                                                                }
                                                                else
                                                                {
                                                                    std::cerr << "[Connect Error] " << ec.message() << "\n";
                                                                    onConnect(false);
                                                                }
                                                            });
                                    });

            // Run io_context in the main thread — keeps client alive
            ioContext_.run();
        }

        // Fully async read/write loop for messages of type T
        template <typename T>
        void startAsyncLoop(std::function<void(const T &received)> onReceive)
        {
            auto readLoop = std::make_shared<std::function<void()>>();
            auto buffer = std::make_shared<std::array<char, sizeof(T)>>();

            *readLoop = [this, buffer, onReceive, readLoop]()
            {
                socket_->async_read_some(
                    asio::buffer(*buffer),
                    [this, buffer, onReceive, readLoop](std::error_code ec, std::size_t len)
                    {
                        if (ec)
                        {
                            std::cerr << "[Read Error] " << ec.message() << "\n";
                            return;
                        }

                        if (len != sizeof(T))
                        {
                            std::cerr << "[Read Error] Expected " << sizeof(T)
                                      << " bytes, got " << len << " bytes.\n";
                            return;
                        }

                        T object;
                        std::memcpy(&object, buffer->data(), sizeof(T));

                        // Call the user-provided handler
                        onReceive(object);

                        // Example: reply automatically
                        auto sendBuffer = std::make_shared<std::array<char, sizeof(T)>>();
                        std::memcpy(sendBuffer->data(), &object, sizeof(T));
                        asio::async_write(*socket_, asio::buffer(*sendBuffer),
                                          [sendBuffer](std::error_code ec, std::size_t)
                                          {
                                              if (ec)
                                                  std::cerr << "[Send Error] " << ec.message() << "\n";
                                          });

                        // Continue reading
                        (*readLoop)();
                    });
            };

            // Start the loop
            (*readLoop)();
        }

        template <typename T>
        void sendData(const T &message)
        {
            auto buffer = std::make_shared<std::array<char, sizeof(T)>>();
            std::memcpy(buffer->data(), &message, sizeof(T));

            asio::async_write(*socket_, asio::buffer(*buffer),
                              [buffer](std::error_code ec, std::size_t)
                              {
                                  if (ec)
                                      std::cerr << "[Send Error] " << ec.message() << "\n";
                              });
        }

    private:
        std::shared_ptr<asio::ip::tcp::socket> socket_;
        asio::io_context ioContext_;
    };

} // namespace Networking