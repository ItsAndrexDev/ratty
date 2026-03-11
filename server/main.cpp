#include "../deps/NetworkManager.h"
#include "../deps/ResponseEntity.h"

int main()
{
    Crilly::ServerManager server_manager(4444);

    for (;;) {
        Crilly::IncomingMessage<Message> message;
        while (server_manager.getNextMessage(message)) {
            std::cout << "processing session " << message.data.session << std::endl;
            message.data.value += 1;
            server_manager.writeData(message.socket, message.data);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}