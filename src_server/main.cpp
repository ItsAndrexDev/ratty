#include "Networking/ServerManager.h"
#include <iostream>
#include <thread>

struct Message
{
    int value;
};

int main()
{
    Networking::ServerManager server;

    unsigned short port = 12345;

    // Start the server (accepts clients asynchronously)
    server.startServer(port);

    std::cout << "Server running on port " << port << ". Press Ctrl+C to quit.\n";

    // Optionally, broadcast a message every 5 seconds
    std::thread([&server]() {
        while (true)
        {
            Message msg{42};
            server.broadcastData(msg);
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }).detach();

    // Keep the main thread alive
    std::this_thread::sleep_for(std::chrono::hours(24));
}