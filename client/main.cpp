#include "../deps/NetworkManager.h"
#include <iostream>
#include <thread>
#include "../deps/ResponseEntity.h"

int main()
{
	Crilly::ClientManager client_manager("127.0.0.1", 4444);
	std::cout << "connected\n";
	Message msg{2,2,Status::FAILURE,Action::UNINSTALL};

	client_manager.writeData(msg);

	Crilly::IncomingMessage<Message> message;
	for (;;)
		if (client_manager.getNextMessage(message)) {
			std::cout << "session: " << message.data.session << std::endl;
			std::cout << "action: " << message.data.action << std::endl;
			std::cout << "value: " << message.data.value << std::endl;
			std::cout << "status: " << message.data.status << std::endl;
			break;
		}

	if (!client_manager.isConnected()) {
		std::cout << "disconnected\n";
		return 0;
	}
	std::cin.get();
}