#include "Networking/ClientManager.h"
#include <thread>
#include <chrono>

int main()
{
    Networking::ClientManager client;

    client.connect("127.0.0.1", 12345, [](bool success) {
        if (!success)
        {
            std::cerr << "Failed to connect to server.\n";
            return;
        }
        std::cout << "Connected!\n";
    });

    // Wait a second to ensure the connection is established
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Send some messages
    Networking::Message msg{42};
    client.sendData(msg);

    // Send more messages periodically
    std::thread([&client]() {
        int i = 0;
        while (true)
        {
            Networking::Message m{i++};
            client.sendData(m);
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }).detach();

    // io_context.run() is already called inside connect(), keeps client alive
}