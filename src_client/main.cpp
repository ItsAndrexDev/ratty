#include "../deps/NetworkManager.h"
#include <iostream>
#include <thread>
int main()
{
	NetworkManagement::ClientManager client_manager("127.0.0.1", 4444);

	client_manager.writeData(123);
}