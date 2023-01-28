#include "net_server.h"
#include <assert.h>
#include <memory>

static uint8_t packet_data[NETCODE_MAX_PACKET_SIZE];

static uint8_t private_key[NETCODE_KEY_BYTES] = { 0x60, 0x6a, 0xbe, 0x6e, 0xc9, 0x19, 0x10, 0xea, 
                                                  0x9a, 0x65, 0x62, 0xf6, 0x6f, 0x2b, 0x30, 0xe4, 
                                                  0x43, 0x71, 0xd6, 0x2c, 0xd1, 0x99, 0x27, 0x26,
                                                  0x6b, 0x3c, 0x60, 0xf4, 0xb7, 0x15, 0xab, 0xa1 };

bool Net_Server::Init(double time) {
    if (netcode_init() != NETCODE_OK) {
        return false;
    }

    netcode_log_level( NETCODE_LOG_LEVEL_INFO );

    // double delta_time = 1.0 / 60.0;

    char * server_address = "127.0.0.1:40000";

    struct netcode_server_config_t server_config;
    netcode_default_server_config( &server_config );
    server_config.protocol_id = PROTOCOL_ID;
    memcpy( &server_config.private_key, private_key, NETCODE_KEY_BYTES );

    server = netcode_server_create( server_address, &server_config, time );

    if (!server) {
        return false;
    }

    netcode_server_start( server, NETCODE_MAX_CLIENTS );

    for (int i = 0; i < NETCODE_MAX_PACKET_SIZE; ++i) {
        packet_data[i] = (uint8_t) i;
    }
    mode = NetKind::DedicatedServer;
    return true;
}

void Net_Server::Update(double time) {
    assert(server != nullptr);
    netcode_server_update(server, time);

    if (netcode_server_client_connected(server, 0)) {
        netcode_server_send_packet(server, 0, packet_data, NETCODE_MAX_PACKET_SIZE);
    }

    for (int client_index = 0; client_index < NETCODE_MAX_CLIENTS; ++client_index) {
        while (1) {
            int packet_bytes;
            uint64_t packet_sequence;
            void * packet = netcode_server_receive_packet(server, client_index, &packet_bytes, &packet_sequence);
            if (!packet) {
                break;
            }
            (void) packet_sequence;
            assert( packet_bytes == NETCODE_MAX_PACKET_SIZE );
            assert( memcmp( packet, packet_data, NETCODE_MAX_PACKET_SIZE ) == 0 );            
            netcode_server_free_packet(server, packet );
        }
    }
}

void Net_Server::Shutdown() {
   netcode_server_destroy(server);
   netcode_term();
   mode = NetKind::Offline;
   server = nullptr;
}
