#include "ClientManager.h"

namespace Networking {

using asio::ip::tcp;

ClientManager::~ClientManager()
{
    if (socket_ && socket_->is_open())
    {
        asio::error_code ec;
        socket_->close(ec);
    }
}


} // namespace Networking