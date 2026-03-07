#include "../deps/NetworkManager.h"
#include <iostream>
#include <thread>
int main()
{
	NetworkManagement::ClientManager client_manager("127.0.0.1", 4444);

	for (;;) {
		int received = client_manager.sendAndReceive<int, int>(41);
		std::cout << "Received: " << received << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}