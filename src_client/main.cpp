#include "../deps/NetworkManager.h"
#include <iostream>
#include <thread>
int main()
{
	NetworkManagement::ClientManager client_manager("127.0.0.1", 4444);

	for (;;) {
        client_manager.writeData<int>(12345,
            [&](bool success)
            {
                if (!success)
                    return;

                client_manager.readData<int>(
                    [&](int received)
                    {
                        std::cout << "Received: " << received << std::endl;

                        if (received == 12345)
                            std::cout << "Data sent successfully!" << std::endl;
                    });
            });
	}
}