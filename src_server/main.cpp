#include "../deps/NetworkManager.h"
#include <iostream>
#include <thread>
int main()
{

    NetworkManagement::ServerManager server_manager(4444);
	for (;;) {
	    std::cout << "reading... \n";

        server_manager.readData<int>(
            [&](int received, auto socket)
            {
                std::cout << "Received: " << received << std::endl;

                server_manager.writeData<int>(socket, received,
                    [](bool success)
                    {
                        if (success)
                            std::cout << "Data sent successfully!" << std::endl;
                    });
            });
	}
}