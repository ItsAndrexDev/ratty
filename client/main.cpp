#include "../deps/NetworkManager.h"
#include <iostream>
#include <thread>
enum Action {
	WATCH = 0,
	LOG,
	EXIT,
	UNINSTALL
};
struct Message {
	int session;
	int value;
	Action action;

	Message() = default;

	Message(int session, int value, Action action) : session(session), value(value), action(action) {};
};



int main()
{
	Crilly::ClientManager client_manager("127.0.0.1", 4444);
	std::cout << "connected\n";
	Message data{11,2,Action::WATCH};
	client_manager.writeData<Message>(data);
	std::cout << "wrote data\n";

	Crilly::IncomingMessage<Message> message;
	bool shouldRun = 1;
	while (shouldRun) {
		std::cout << "running\n";
		while (client_manager.getNextMessage(message)) {
			std::cout << "session:" << message.data.session << std::endl;
			std::cout << "value:" << message.data.value << std::endl;
			std::cout << "action:" << message.data.action << std::endl;
			shouldRun = 0;
		}

		if (!client_manager.isConnected()) {
			std::cout << "disconnected\n";
			return 0;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}




}