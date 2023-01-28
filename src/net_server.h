#ifndef NET_SERVER_H
#define NET_SERVER_H

#include "net_interface.h"
#include "netcode.h"

class Net_Server : public Net_Interface {
public:
    bool Init(double time);
    void Update(double time);
    void Shutdown();
private:
    netcode_server_t* server;
};

#endif
