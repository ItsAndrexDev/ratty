#include "../deps/NetworkManager.h"
#include <iostream>
#include <thread>
#include "../deps/ResponseEntity.h"

int main()
{
	Crilly::ClientManager client_manager("127.0.0.1", 4444);
	std::cout << "connected\n";

	Crilly::IncomingMessage<Message> message;
	bool shouldRun = 1;
	while (shouldRun) {
		std::cout << "running\n";
		while (client_manager.getNextMessage(message)) {
			client_manager.writeData(message);
		}

		if (!client_manager.isConnected()) {
			std::cout << "disconnected\n";
			return 0;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}