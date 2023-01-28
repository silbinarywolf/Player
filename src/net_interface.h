#ifndef NET_INTERFACE_H
#define NET_INTERFACE_H

#define PROTOCOL_ID 0x1122334455667788

/** NetKind is the current netcode mode */
enum NetKind {
    Offline = 0,
    DedicatedServer = 1,
    ListenServer = 2,
    Client = 3
};

class Net_Interface {
public:
    virtual bool Init(double time) = 0;
    virtual void Update(double time) = 0;
    virtual void Shutdown() = 0;

    inline NetKind Kind() {
        return mode;
    }

	inline bool IsDisconnected() {
		return is_disconnected;
	}
protected:
    NetKind mode = NetKind::Offline;
    bool is_disconnected = false;
};

#endif
