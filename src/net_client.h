#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include "net_interface.h"
#include "netcode.h"

class Net_Client : public Net_Interface {
public:
    bool Init(double time);
    void Update(double time);
    void Shutdown();
private:
    netcode_client_t * client;
};

#endif
