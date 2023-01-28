#ifndef NET_INTERFACE_H
#define NET_INTERFACE_H

#define PROTOCOL_ID 0x1122334455667788

class Net_Interface {
public:
    virtual bool Init(double time) = 0;
    virtual void Update(double time) = 0;
    virtual void Shutdown() = 0;

	inline bool IsDisconnected() {
		return is_disconnected;
	}
protected:
    bool is_disconnected = false;
};

#endif
