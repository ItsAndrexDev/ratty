#pragma once

namespace NetworkManagement
{
    class Servermanager
    {
    private:
    
    public:
        Servermanager(unsigned short port);
        ~Servermanager();
    };

    class ClientManager
    {
    private:
        bool connectToHost();    


    public:
        ClientManager(char ip[15], unsigned short port);
        ~ClientManager();
    };

} // namespace NetworkManagement
